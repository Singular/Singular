/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    ssi2Link.cc
 * Purpose: binary SSI2 link implementation
 ***************************************************************/
#define TRANSEXT_PRIVATES 1 /* allow access to transext internals */

#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "misc/options.h"

#include "reporter/si_signals.h"
#include "reporter/s_buff.h"
#include "reporter/si_signals.h"

#include "coeffs/bigintmat.h"
#include "coeffs/longrat.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/ext_fields/transext.h"
#include "polys/simpleideals.h"
#include "polys/matpol.h"

#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/timer.h"
#include "kernel/oswrapper/feread.h"
#include "kernel/oswrapper/rlimit.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/subexpr.h"
#include "Singular/links/silink.h"
#include "Singular/cntrlc.h"
#include "Singular/feOpt.h"
#include "Singular/lists.h"
#include "Singular/htable.h"
#include "Singular/blackbox.h"
#include "Singular/links/ssi2Link.h"

#ifdef HAVE_SIMPLEIPC
#include "Singular/links/simpleipc.h"
#endif

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>          /* for portability */
#include <ctype.h>   /*for isdigit*/
#include <netdb.h>
#include <netinet/in.h> /* for htons etc.*/

#ifdef HAVE_LIBSODIUM
#include <sodium.h>
#endif

#ifdef HAVE_OPENSSL_FIPS
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/provider.h>
#include <openssl/rand.h>
#endif

EXTERN_VAR BOOLEAN FE_OPT_NO_SHELL_FLAG;

static BOOLEAN ssi2SetCurrRing(const ring r)
{
  if (r==currRing)
  {
    rIncRefCnt(r);
    currRingHdl=rFindHdl(r,currRingHdl);
    return TRUE;
  }
  if ((currRing==NULL) || (!rEqual(r,currRing,1)))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;
        break;
      }
      if ((IDTYP(h)==RING_CMD) && (rEqual(r,IDRING(h),1)))
      {
        rIncRefCnt(IDRING(h));
        break;
      }
    }
    rSetHdl(h);
    return FALSE;
  }

  rKill(r);
  rIncRefCnt(currRing);
  return TRUE;
}

static void ssi2CheckCurrRing(const ring r)
{
  if ((r!=currRing) || (currRingHdl==NULL) || (IDRING(currRingHdl)!=r))
  {
    char name[20];
    int nr=0;
    idhdl h=NULL;
    loop
    {
      snprintf(name,20,"ssiRing%d",nr); nr++;
      h=IDROOT->get(name, 0);
      if (h==NULL)
      {
        h=enterid(name,0,RING_CMD,&IDROOT,FALSE);
        IDRING(h)=rIncRefCnt(r);
        r->ref=2;
        break;
      }
      if ((IDTYP(h)==RING_CMD) && (rEqual(r,IDRING(h),1))) break;
    }
    rSetHdl(h);
  }
  assume((currRing==r) || rEqual(r,currRing));
}

#define SSI2_VERSION 1
#define SSI_SCHEMA_TOKEN 26
#define SSI_SCHEMA_TABLE_VERSION 1
#define SSI2_SCHEMA_VERSION_COUNT 32

struct ssi2Info : public ssiInfo
{
  char *write_buff;
  int write_buff_pos;
  int write_buff_size;
  const char *compressor_name;
  unsigned short schema_versions[SSI2_SCHEMA_VERSION_COUNT];
  uint32_t validated_schema_versions;
  BOOLEAN read_failed;
  BOOLEAN write_failed;
  BOOLEAN header_seen;
  BOOLEAN schema_table_seen;
  int read_depth;
#ifdef HAVE_LIBSODIUM
  BOOLEAN encrypted;
  BOOLEAN encryption_reading;
  BOOLEAN encryption_failed;
  BOOLEAN encryption_final_seen;
  BOOLEAN encryption_final_written;
  FILE *encryption_input;
  crypto_secretstream_xchacha20poly1305_state encryption_state;
  unsigned char encryption_header[12 + crypto_secretstream_xchacha20poly1305_HEADERBYTES];
  uint64_t encryption_frame;
  unsigned char *encryption_read_buff;
  size_t encryption_read_pos;
  size_t encryption_read_len;
#endif
#ifdef HAVE_OPENSSL_FIPS
  BOOLEAN openssl_encrypted;
  BOOLEAN openssl_reading;
  BOOLEAN openssl_failed;
  BOOLEAN openssl_final_seen;
  BOOLEAN openssl_final_written;
  FILE *openssl_input;
  OSSL_LIB_CTX *openssl_libctx;
  OSSL_PROVIDER *openssl_base_provider;
  OSSL_PROVIDER *openssl_fips_provider;
  EVP_CIPHER *openssl_cipher;
  unsigned char openssl_key[32];
  unsigned char openssl_header[24];
  uint64_t openssl_frame;
  unsigned char *openssl_read_buff;
  size_t openssl_read_pos;
  size_t openssl_read_len;
#endif
};

enum ssiSchemaId
{
  SSI_SCHEMA_NUMBER=1,
  SSI_SCHEMA_RING,
  SSI_SCHEMA_POLY,
  SSI_SCHEMA_IDEAL,
  SSI_SCHEMA_MATRIX,
  SSI_SCHEMA_MODULE,
  SSI_SCHEMA_LIST,
  SSI_SCHEMA_PROC,
  SSI_SCHEMA_INTVEC,
  SSI_SCHEMA_BIGINTMAT,
  SSI_SCHEMA_ATTRIBUTES,
  SSI_SCHEMA_COMMAND,
  SSI_SCHEMA_RING_PROPERTIES,
  SSI_SCHEMA_BLACKBOX,
  SSI_SCHEMA_HTABLE,
  SSI_SCHEMA_MPZ
};

struct ssiSchemaVersionEntry
{
  int id;
  int version;
  const char *name;
};

static const ssiSchemaVersionEntry ssiSchemaVersions[] =
{
  { SSI_SCHEMA_NUMBER,          1, "number" },
  { SSI_SCHEMA_RING,            1, "ring" },
  { SSI_SCHEMA_POLY,            1, "polynomial" },
  { SSI_SCHEMA_IDEAL,           1, "ideal" },
  { SSI_SCHEMA_MATRIX,          1, "matrix" },
  { SSI_SCHEMA_MODULE,          1, "module" },
  { SSI_SCHEMA_LIST,            1, "list" },
  { SSI_SCHEMA_PROC,            1, "proc" },
  { SSI_SCHEMA_INTVEC,          1, "intvec" },
  { SSI_SCHEMA_BIGINTMAT,       1, "bigintmat" },
  { SSI_SCHEMA_ATTRIBUTES,      1, "attributes" },
  // Bump this if the serialized command token mapping changes.
  { SSI_SCHEMA_COMMAND,         1, "command" },
  { SSI_SCHEMA_RING_PROPERTIES, 1, "ring-properties" },
  { SSI_SCHEMA_BLACKBOX,        1, "blackbox" },
  { SSI_SCHEMA_HTABLE,          1, "htable" },
  { SSI_SCHEMA_MPZ,             1, "mpz" }
};

static const int ssiSchemaVersionCount =
  (int)(sizeof(ssiSchemaVersions)/sizeof(ssiSchemaVersions[0]));

static int ssiCurrentSchemaVersion(int id)
{
  for (int i=0; i<ssiSchemaVersionCount; i++)
    if (ssiSchemaVersions[i].id==id) return ssiSchemaVersions[i].version;
  return 0;
}

static const char *ssiSchemaName(int id)
{
  for (int i=0; i<ssiSchemaVersionCount; i++)
    if (ssiSchemaVersions[i].id==id) return ssiSchemaVersions[i].name;
  return "unknown";
}

static void ssiInitSchemaVersions(ssiInfo *d, BOOLEAN for_write)
{
  if (d==NULL) return;
  ssi2Info *dd=(ssi2Info*)d;
  memset(dd->schema_versions, 0, sizeof(dd->schema_versions));
  dd->validated_schema_versions=0;
  if (!for_write) return;
  for (int i=0; i<ssiSchemaVersionCount; i++)
  {
    int id=ssiSchemaVersions[i].id;
    if ((id>0) && (id<SSI2_SCHEMA_VERSION_COUNT))
      dd->schema_versions[id]=(unsigned short)ssiSchemaVersions[i].version;
  }
}

static int ssiSchemaVersion(const ssiInfo *d, int id)
{
  const ssi2Info *dd=(const ssi2Info*)d;
  if ((dd!=NULL) && (id>0) && (id<SSI2_SCHEMA_VERSION_COUNT)
  && (dd->schema_versions[id]!=0))
    return dd->schema_versions[id];

  // The original SSI2 schema table did not list the MPZ encoding separately.
  if ((dd!=NULL) && dd->schema_table_seen && (id==SSI_SCHEMA_MPZ)) return 1;
  return 0;
}

static void ssiSetSchemaVersion(ssiInfo *d, int id, int version)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd!=NULL) && (id>0) && (id<SSI2_SCHEMA_VERSION_COUNT) && (version>0))
    dd->schema_versions[id]=(unsigned short)version;
}

static BOOLEAN ssiRequireSchemaVersion(const ssiInfo *d, int id,
                                       const char *format)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((id>0) && (id<SSI2_SCHEMA_VERSION_COUNT)
  && ((dd->validated_schema_versions & (1U << id))!=0)) return FALSE;
  int have=ssiSchemaVersion(d, id);
  int current=ssiCurrentSchemaVersion(id);
  if ((current<=0) || (have!=current))
  {
    Werror("%s: unsupported %s schema version %d (expected %d)",
           format, ssiSchemaName(id), have, current);
    dd->read_failed=TRUE;
    return TRUE;
  }
  dd->validated_schema_versions|=(1U << id);
  return FALSE;
}

static BOOLEAN ssi2Write(si_link l, leftv data);
static leftv ssi2Read1(si_link l);
static leftv ssi2Read1Internal(si_link l);
static void ssi2WriteRing(ssiInfo *d, const ring r);
static void ssi2WriteRing_R(ssiInfo *d, const ring r);
static ring ssi2ReadRing(ssiInfo *d);
static void ssi2WritePoly_R(const ssiInfo *d, poly p, const ring r);
static poly ssi2ReadPoly_R(const ssiInfo *d, const ring r);
static void ssi2WriteIdeal_R(const ssiInfo *d, int typ, const ideal I, const ring r);
static ideal ssi2ReadIdeal_R(const ssiInfo *d, const ring r);
static matrix ssi2ReadMatrix(ssiInfo *d);
static BOOLEAN ssi2zClose(si_link l);

enum ssi2Compression
{
  SSI2_COMP_NONE,
  SSI2_COMP_GZIP,
  SSI2_COMP_ZSTD,
  SSI2_COMP_LZ4,
  SSI2_COMP_INVALID
};

static BOOLEAN ssi2ModeTokenEquals(const char *s, int len, const char *token)
{
  return ((int)strlen(token)==len) && (strncmp(s, token, len)==0);
}

static char ssi2ModeBase(const char *mode)
{
  if ((mode==NULL) || (mode[0]=='\0')) return '\0';
  if (((mode[0]=='r') || (mode[0]=='w') || (mode[0]=='a'))
  && ((mode[1]=='\0') || (mode[1]==',')))
    return mode[0];
  return '?';
}

static char *ssi2ReopenMode(const char *mode, char base)
{
  if ((mode==NULL) || (mode[0]=='\0'))
  {
    char result[2]={base, '\0'};
    return omStrDup(result);
  }
  char *result=omStrDup(mode);
  result[0]=base;
  return result;
}

static BOOLEAN ssi2EndsWith(const char *s, const char *suffix)
{
  size_t slen=strlen(s);
  size_t tlen=strlen(suffix);
  return (slen>=tlen) && (strcmp(s+slen-tlen, suffix)==0);
}

static ssi2Compression ssi2CompressionFromFilename(const char *filename)
{
  if (filename==NULL) return SSI2_COMP_NONE;
  if (ssi2EndsWith(filename, ".gz") || ssi2EndsWith(filename, ".gzip"))
    return SSI2_COMP_GZIP;
  if (ssi2EndsWith(filename, ".zst") || ssi2EndsWith(filename, ".zstd"))
    return SSI2_COMP_ZSTD;
  if (ssi2EndsWith(filename, ".lz4"))
    return SSI2_COMP_LZ4;
  return SSI2_COMP_NONE;
}

static ssi2Compression ssi2ParseModeOptions(const char *mode, char *zstd_long,
                                            int zstd_long_size,
                                            BOOLEAN *has_long,
                                            BOOLEAN *has_compression)
{
  ssi2Compression comp=SSI2_COMP_NONE;
  if (has_long!=NULL) *has_long=FALSE;
  if (has_compression!=NULL) *has_compression=FALSE;
  if (zstd_long!=NULL) strncpy(zstd_long, "--long=23", zstd_long_size);
  char base=ssi2ModeBase(mode);
  if (base=='?')
  {
    Werror("ssi2: invalid mode `%s'", mode);
    return SSI2_COMP_INVALID;
  }
  if ((mode==NULL) || (mode[0]=='\0')) return comp;
  const char *p=strchr(mode, ',');
  while (p!=NULL)
  {
    const char *start=p+1;
    const char *end=strchr(start, ',');
    int len=(end==NULL) ? (int)strlen(start) : (int)(end-start);
    if (len==0)
    {
      Werror("ssi2: empty mode option in `%s'", mode);
      return SSI2_COMP_INVALID;
    }
    ssi2Compression next=SSI2_COMP_NONE;
    if (ssi2ModeTokenEquals(start, len, "gzip")) next=SSI2_COMP_GZIP;
    else if (ssi2ModeTokenEquals(start, len, "zstd")) next=SSI2_COMP_ZSTD;
    else if (ssi2ModeTokenEquals(start, len, "lz4")) next=SSI2_COMP_LZ4;
    else if (ssi2ModeTokenEquals(start, len, "plain")
          || ssi2ModeTokenEquals(start, len, "none"))
    {
      if (has_compression!=NULL) *has_compression=TRUE;
    }
    else if ((len>5) && (strncmp(start, "long=", 5)==0))
    {
      for (int i=5; i<len; i++)
      {
        if (!isdigit((unsigned char)start[i]))
        {
          Werror("ssi2: invalid zstd long window option `%.*s'", len, start);
          return SSI2_COMP_INVALID;
        }
      }
      if (has_long!=NULL) *has_long=TRUE;
      if ((zstd_long!=NULL) && (zstd_long_size>0))
      {
        int n=(len+2<zstd_long_size) ? len : zstd_long_size-3;
        zstd_long[0]='-';
        zstd_long[1]='-';
        strncpy(zstd_long+2, start, n);
        zstd_long[n+2]='\0';
      }
    }
    else
    {
      Werror("ssi2: unknown mode option `%.*s'", len, start);
      return SSI2_COMP_INVALID;
    }
    if (next!=SSI2_COMP_NONE)
    {
      if (has_compression!=NULL) *has_compression=TRUE;
      if ((comp!=SSI2_COMP_NONE) && (comp!=next))
      {
        Werror("ssi2: multiple compression options in mode `%s'", mode);
        return SSI2_COMP_INVALID;
      }
      comp=next;
    }
    p=end;
  }
  return comp;
}

static BOOLEAN ssi2CompressedOpenByCompression(si_link l, short flag,
                                               ssi2Compression comp,
                                               const char *zstd_long);

#ifdef HAVE_LIBSODIUM
#define SSI2E_FIXED_HEADER_SIZE 12
#define SSI2E_FRAME_AD_SIZE \
  (SSI2E_FIXED_HEADER_SIZE + crypto_secretstream_xchacha20poly1305_HEADERBYTES + 8 + 4)
#define SSI2E_PLAINTEXT_CHUNK_SIZE (1U << 20)

static const unsigned char ssi2eFixedHeader[SSI2E_FIXED_HEADER_SIZE] =
{
  'S', 'S', 'I', '2', 'E', 'N', 'C', 0,
  1, /* envelope version */
  1, /* XChaCha20-Poly1305 secretstream */
  0, 0
};

static void ssi2eStoreU32(unsigned char *p, uint32_t v)
{
  p[0]=(unsigned char)(v >> 24);
  p[1]=(unsigned char)(v >> 16);
  p[2]=(unsigned char)(v >> 8);
  p[3]=(unsigned char)v;
}

static uint32_t ssi2eLoadU32(const unsigned char *p)
{
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
       | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void ssi2eStoreU64(unsigned char *p, uint64_t v)
{
  for (int i=7; i>=0; i--)
  {
    p[i]=(unsigned char)v;
    v >>= 8;
  }
}

static void ssi2eFrameAssociatedData(const ssi2Info *d, uint32_t frame_len,
                                    unsigned char *ad)
{
  memcpy(ad, d->encryption_header,
         SSI2E_FIXED_HEADER_SIZE + crypto_secretstream_xchacha20poly1305_HEADERBYTES);
  unsigned char *p=ad + SSI2E_FIXED_HEADER_SIZE
                    + crypto_secretstream_xchacha20poly1305_HEADERBYTES;
  ssi2eStoreU64(p, d->encryption_frame);
  ssi2eStoreU32(p+8, frame_len);
}

static BOOLEAN ssi2eReadKeyFile(const char *keyfile,
                                unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])
{
  FILE *f=fopen(keyfile, "rb");
  if (f==NULL)
  {
    WerrorS("ssi2e: cannot open key file");
    return TRUE;
  }

  char hex[crypto_secretstream_xchacha20poly1305_KEYBYTES*2 + 1];
  size_t n=0;
  int c;
  BOOLEAN invalid=FALSE;
  while ((c=fgetc(f))!=EOF)
  {
    if (isspace((unsigned char)c)) continue;
    if ((!isxdigit((unsigned char)c)) || (n>=sizeof(hex)-1))
    {
      invalid=TRUE;
      break;
    }
    hex[n++]=(char)c;
  }
  if (ferror(f)) invalid=TRUE;
  if (fclose(f)!=0) invalid=TRUE;
  hex[n]='\0';

  size_t key_len=0;
  if (invalid || (n!=sizeof(hex)-1)
  || (sodium_hex2bin(key, crypto_secretstream_xchacha20poly1305_KEYBYTES,
                     hex, n, NULL, &key_len, NULL)!=0)
  || (key_len!=crypto_secretstream_xchacha20poly1305_KEYBYTES))
  {
    sodium_memzero(hex, sizeof(hex));
    sodium_memzero(key, crypto_secretstream_xchacha20poly1305_KEYBYTES);
    WerrorS("ssi2e: key file must contain exactly 64 hexadecimal characters");
    return TRUE;
  }
  sodium_memzero(hex, sizeof(hex));
  return FALSE;
}

static BOOLEAN ssi2eParseModeOptions(const char *mode, char **keyfile)
{
  *keyfile=NULL;
  char base=ssi2ModeBase(mode);
  if (base=='?')
  {
    Werror("ssi2e: invalid mode `%s'", mode);
    return TRUE;
  }
  if ((mode==NULL) || (mode[0]=='\0'))
  {
    WerrorS("ssi2e: keyfile= mode option is required");
    return TRUE;
  }

  const char *p=strchr(mode, ',');
  while (p!=NULL)
  {
    const char *start=p+1;
    const char *end=strchr(start, ',');
    int len=(end==NULL) ? (int)strlen(start) : (int)(end-start);
    if ((len>8) && (strncmp(start, "keyfile=", 8)==0))
    {
      if (*keyfile!=NULL)
      {
        WerrorS("ssi2e: keyfile= may only be specified once");
        omFree(*keyfile);
        *keyfile=NULL;
        return TRUE;
      }
      *keyfile=(char*)omAlloc((size_t)len-7);
      memcpy(*keyfile, start+8, (size_t)len-8);
      (*keyfile)[len-8]='\0';
    }
    else
    {
      Werror("ssi2e: unknown or empty mode option `%.*s'", len, start);
      if (*keyfile!=NULL) omFree(*keyfile);
      *keyfile=NULL;
      return TRUE;
    }
    p=end;
  }
  if (*keyfile==NULL)
  {
    WerrorS("ssi2e: keyfile= mode option is required");
    return TRUE;
  }
  return FALSE;
}

static BOOLEAN ssi2eWriteFrame(ssi2Info *d, const unsigned char *plain,
                               size_t plain_len, unsigned char tag)
{
  if (d->encryption_failed) return TRUE;
  if (d->encryption_frame==UINT64_MAX)
  {
    WerrorS("ssi2e: encrypted stream has too many frames");
    d->encryption_failed=TRUE;
    return TRUE;
  }
  if ((plain_len>SSI2E_PLAINTEXT_CHUNK_SIZE)
  || (plain_len>UINT32_MAX-crypto_secretstream_xchacha20poly1305_ABYTES))
  {
    WerrorS("ssi2e: internal frame size error");
    d->encryption_failed=TRUE;
    return TRUE;
  }

  uint32_t frame_len=(uint32_t)plain_len
                   + crypto_secretstream_xchacha20poly1305_ABYTES;
  unsigned char prefix[4];
  unsigned char ad[SSI2E_FRAME_AD_SIZE];
  ssi2eStoreU32(prefix, frame_len);
  ssi2eFrameAssociatedData(d, frame_len, ad);

  unsigned char *cipher=(unsigned char*)omAlloc(frame_len);
  unsigned long long cipher_len=0;
  int crypto_result=crypto_secretstream_xchacha20poly1305_push(
    &d->encryption_state, cipher, &cipher_len, plain, plain_len,
    ad, sizeof(ad), tag);
  if ((crypto_result!=0) || (cipher_len!=frame_len)
  || (fwrite(prefix, 1, sizeof(prefix), d->f_write)!=sizeof(prefix))
  || (fwrite(cipher, 1, frame_len, d->f_write)!=frame_len))
  {
    sodium_memzero(cipher, frame_len);
    omFreeSize(cipher, frame_len);
    WerrorS("ssi2e: encrypted write failed");
    d->encryption_failed=TRUE;
    return TRUE;
  }
  sodium_memzero(cipher, frame_len);
  omFreeSize(cipher, frame_len);
  d->encryption_frame++;
  return FALSE;
}

static BOOLEAN ssi2eReadFrame(ssi2Info *d)
{
  if (d->encryption_failed || d->encryption_final_seen) return TRUE;
  if (d->encryption_frame==UINT64_MAX)
  {
    WerrorS("ssi2e: encrypted stream has too many frames");
    d->encryption_failed=TRUE;
    return TRUE;
  }

  unsigned char prefix[4];
  if (fread(prefix, 1, sizeof(prefix), d->encryption_input)!=sizeof(prefix))
  {
    WerrorS("ssi2e: truncated encrypted stream (missing final frame)");
    d->encryption_failed=TRUE;
    return TRUE;
  }
  uint32_t frame_len=ssi2eLoadU32(prefix);
  if ((frame_len<crypto_secretstream_xchacha20poly1305_ABYTES)
  || (frame_len>SSI2E_PLAINTEXT_CHUNK_SIZE
                 + crypto_secretstream_xchacha20poly1305_ABYTES))
  {
    WerrorS("ssi2e: invalid encrypted frame length");
    d->encryption_failed=TRUE;
    return TRUE;
  }

  unsigned char *cipher=(unsigned char*)omAlloc(frame_len);
  if (fread(cipher, 1, frame_len, d->encryption_input)!=frame_len)
  {
    sodium_memzero(cipher, frame_len);
    omFreeSize(cipher, frame_len);
    WerrorS("ssi2e: truncated encrypted frame");
    d->encryption_failed=TRUE;
    return TRUE;
  }

  unsigned char ad[SSI2E_FRAME_AD_SIZE];
  ssi2eFrameAssociatedData(d, frame_len, ad);
  unsigned long long plain_len=0;
  unsigned char tag=0;
  int crypto_result=crypto_secretstream_xchacha20poly1305_pull(
    &d->encryption_state, d->encryption_read_buff, &plain_len, &tag,
    cipher, frame_len, ad, sizeof(ad));
  sodium_memzero(cipher, frame_len);
  omFreeSize(cipher, frame_len);
  if (crypto_result!=0)
  {
    WerrorS("ssi2e: authentication failed (wrong key or modified data)");
    d->encryption_failed=TRUE;
    return TRUE;
  }
  d->encryption_frame++;

  if (tag==crypto_secretstream_xchacha20poly1305_TAG_FINAL)
  {
    if (plain_len!=0)
    {
      WerrorS("ssi2e: invalid final frame");
      d->encryption_failed=TRUE;
      return TRUE;
    }
    int trailing=fgetc(d->encryption_input);
    if ((trailing!=EOF) || ferror(d->encryption_input))
    {
      WerrorS("ssi2e: trailing data after final frame");
      d->encryption_failed=TRUE;
      return TRUE;
    }
    d->encryption_final_seen=TRUE;
    d->encryption_read_pos=0;
    d->encryption_read_len=0;
    return FALSE;
  }
  if ((tag!=crypto_secretstream_xchacha20poly1305_TAG_MESSAGE)
  || (plain_len==0) || (plain_len>SSI2E_PLAINTEXT_CHUNK_SIZE))
  {
    WerrorS("ssi2e: unsupported encrypted frame tag");
    d->encryption_failed=TRUE;
    return TRUE;
  }
  d->encryption_read_pos=0;
  d->encryption_read_len=(size_t)plain_len;
  return FALSE;
}

static BOOLEAN ssi2eReadRaw(ssi2Info *d, void *buf, size_t len)
{
  unsigned char *p=(unsigned char*)buf;
  while (len>0)
  {
    if (d->encryption_read_pos<d->encryption_read_len)
    {
      size_t available=d->encryption_read_len-d->encryption_read_pos;
      size_t take=(len<available) ? len : available;
      memcpy(p, d->encryption_read_buff+d->encryption_read_pos, take);
      d->encryption_read_pos+=take;
      p+=take;
      len-=take;
      continue;
    }
    if (d->encryption_final_seen)
    {
      WerrorS("ssi2: unexpected end of input");
      return TRUE;
    }
    if (ssi2eReadFrame(d)) return TRUE;
  }
  return FALSE;
}

static BOOLEAN ssi2eDrain(ssi2Info *d)
{
  while ((!d->encryption_final_seen) && (!d->encryption_failed))
  {
    d->encryption_read_pos=d->encryption_read_len;
    if (ssi2eReadFrame(d)) break;
  }
  return d->encryption_failed;
}
#endif

#ifdef HAVE_OPENSSL_FIPS
#define SSI2F_FIXED_HEADER_SIZE 16
#define SSI2F_NONCE_PREFIX_SIZE 8
#define SSI2F_HEADER_SIZE (SSI2F_FIXED_HEADER_SIZE + SSI2F_NONCE_PREFIX_SIZE)
#define SSI2F_NONCE_SIZE 12
#define SSI2F_TAG_SIZE 16
#define SSI2F_KEY_SIZE 32
#define SSI2F_FRAME_AD_SIZE (SSI2F_HEADER_SIZE + 8 + 4 + 1)
#define SSI2F_PLAINTEXT_CHUNK_SIZE (1U << 20)

static const unsigned char ssi2fFixedHeader[SSI2F_FIXED_HEADER_SIZE] =
{
  'S', 'S', 'I', '2', 'F', 'I', 'P', 'S',
  1, /* envelope version */
  1, /* AES-256-GCM */
  SSI2F_NONCE_SIZE,
  SSI2F_TAG_SIZE,
  0, 0, 0, 0
};

static void ssi2fStoreU32(unsigned char *p, uint32_t v)
{
  p[0]=(unsigned char)(v >> 24);
  p[1]=(unsigned char)(v >> 16);
  p[2]=(unsigned char)(v >> 8);
  p[3]=(unsigned char)v;
}

static uint32_t ssi2fLoadU32(const unsigned char *p)
{
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
       | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static void ssi2fStoreU64(unsigned char *p, uint64_t v)
{
  for (int i=7; i>=0; i--)
  {
    p[i]=(unsigned char)v;
    v >>= 8;
  }
}

static int ssi2fHexValue(int c)
{
  if ((c>='0') && (c<='9')) return c-'0';
  if ((c>='a') && (c<='f')) return c-'a'+10;
  if ((c>='A') && (c<='F')) return c-'A'+10;
  return -1;
}

static BOOLEAN ssi2fReadKeyFile(const char *keyfile,
                                unsigned char key[SSI2F_KEY_SIZE])
{
  FILE *f=fopen(keyfile, "rb");
  if (f==NULL)
  {
    WerrorS("ssi2f: cannot open key file");
    return TRUE;
  }

  char hex[SSI2F_KEY_SIZE*2 + 1];
  size_t n=0;
  int c;
  BOOLEAN invalid=FALSE;
  while ((c=fgetc(f))!=EOF)
  {
    if (isspace((unsigned char)c)) continue;
    if ((!isxdigit((unsigned char)c)) || (n>=sizeof(hex)-1))
    {
      invalid=TRUE;
      break;
    }
    hex[n++]=(char)c;
  }
  if (ferror(f)) invalid=TRUE;
  if (fclose(f)!=0) invalid=TRUE;
  hex[n]='\0';

  if ((!invalid) && (n==SSI2F_KEY_SIZE*2))
  {
    for (size_t i=0; i<SSI2F_KEY_SIZE; i++)
    {
      int hi=ssi2fHexValue(hex[2*i]);
      int lo=ssi2fHexValue(hex[2*i+1]);
      if ((hi<0) || (lo<0))
      {
        invalid=TRUE;
        break;
      }
      key[i]=(unsigned char)((hi << 4) | lo);
    }
  }
  else invalid=TRUE;

  OPENSSL_cleanse(hex, sizeof(hex));
  if (invalid)
  {
    OPENSSL_cleanse(key, SSI2F_KEY_SIZE);
    WerrorS("ssi2f: key file must contain exactly 64 hexadecimal characters");
    return TRUE;
  }
  return FALSE;
}

static BOOLEAN ssi2fParseModeOptions(const char *mode, char **keyfile)
{
  *keyfile=NULL;
  char base=ssi2ModeBase(mode);
  if (base=='?')
  {
    Werror("ssi2f: invalid mode `%s'", mode);
    return TRUE;
  }
  if ((mode==NULL) || (mode[0]=='\0'))
  {
    WerrorS("ssi2f: keyfile= mode option is required");
    return TRUE;
  }

  const char *p=strchr(mode, ',');
  while (p!=NULL)
  {
    const char *start=p+1;
    const char *end=strchr(start, ',');
    int len=(end==NULL) ? (int)strlen(start) : (int)(end-start);
    if ((len>8) && (strncmp(start, "keyfile=", 8)==0))
    {
      if (*keyfile!=NULL)
      {
        WerrorS("ssi2f: keyfile= may only be specified once");
        omFree(*keyfile);
        *keyfile=NULL;
        return TRUE;
      }
      *keyfile=(char*)omAlloc((size_t)len-7);
      memcpy(*keyfile, start+8, (size_t)len-8);
      (*keyfile)[len-8]='\0';
    }
    else
    {
      Werror("ssi2f: unknown or empty mode option `%.*s'", len, start);
      if (*keyfile!=NULL) omFree(*keyfile);
      *keyfile=NULL;
      return TRUE;
    }
    p=end;
  }
  if (*keyfile==NULL)
  {
    WerrorS("ssi2f: keyfile= mode option is required");
    return TRUE;
  }
  return FALSE;
}

static void ssi2fReportOpenSSLError(const char *operation)
{
  unsigned long err=ERR_get_error();
  if (err==0)
  {
    Werror("ssi2f: %s failed", operation);
    return;
  }

  char buf[256];
  ERR_error_string_n(err, buf, sizeof(buf));
  Werror("ssi2f: %s failed: %s", operation, buf);
  ERR_clear_error();
}

static BOOLEAN ssi2fInitOpenSSL(ssi2Info *d)
{
  d->openssl_libctx=OSSL_LIB_CTX_new();
  if (d->openssl_libctx==NULL)
  {
    WerrorS("ssi2f: cannot create OpenSSL library context");
    return TRUE;
  }
  if (OSSL_PROVIDER_set_default_search_path(d->openssl_libctx,
                                            SSI2_OPENSSL_FIPS_PROVIDER_DIR)!=1)
  {
    WerrorS("ssi2f: cannot set OpenSSL FIPS provider path");
    return TRUE;
  }
  if (OSSL_LIB_CTX_load_config(d->openssl_libctx, SSI2_OPENSSL_FIPS_CONFIG)!=1)
  {
    WerrorS("ssi2f: cannot load OpenSSL FIPS configuration");
    return TRUE;
  }
  d->openssl_base_provider=OSSL_PROVIDER_load(d->openssl_libctx, "base");
  if (d->openssl_base_provider==NULL)
  {
    WerrorS("ssi2f: cannot load OpenSSL base provider");
    return TRUE;
  }
  d->openssl_fips_provider=OSSL_PROVIDER_load(d->openssl_libctx, "fips");
  if (d->openssl_fips_provider==NULL)
  {
    WerrorS("ssi2f: cannot load OpenSSL FIPS provider");
    return TRUE;
  }
  if (EVP_set_default_properties(d->openssl_libctx, "fips=yes")!=1)
  {
    WerrorS("ssi2f: cannot enable OpenSSL FIPS properties");
    return TRUE;
  }
  d->openssl_cipher=EVP_CIPHER_fetch(d->openssl_libctx, "AES-256-GCM", "fips=yes");
  if (d->openssl_cipher==NULL)
  {
    WerrorS("ssi2f: AES-256-GCM is not available from the OpenSSL FIPS provider");
    return TRUE;
  }
  return FALSE;
}

static void ssi2fCleanupOpenSSL(ssi2Info *d)
{
  if (d==NULL) return;
  if (d->openssl_cipher!=NULL)
  {
    EVP_CIPHER_free(d->openssl_cipher);
    d->openssl_cipher=NULL;
  }
  if (d->openssl_fips_provider!=NULL)
  {
    OSSL_PROVIDER_unload(d->openssl_fips_provider);
    d->openssl_fips_provider=NULL;
  }
  if (d->openssl_base_provider!=NULL)
  {
    OSSL_PROVIDER_unload(d->openssl_base_provider);
    d->openssl_base_provider=NULL;
  }
  if (d->openssl_libctx!=NULL)
  {
    OSSL_LIB_CTX_free(d->openssl_libctx);
    d->openssl_libctx=NULL;
  }
  ERR_clear_error();
}

static void ssi2fFrameAssociatedData(const ssi2Info *d, uint32_t frame_len,
                                     BOOLEAN final_frame, unsigned char *ad)
{
  memcpy(ad, d->openssl_header, SSI2F_HEADER_SIZE);
  unsigned char *p=ad + SSI2F_HEADER_SIZE;
  ssi2fStoreU64(p, d->openssl_frame);
  ssi2fStoreU32(p+8, frame_len);
  p[12]=final_frame ? 1 : 0;
}

static void ssi2fFrameNonce(const ssi2Info *d, unsigned char *nonce)
{
  memcpy(nonce, d->openssl_header + SSI2F_FIXED_HEADER_SIZE,
         SSI2F_NONCE_PREFIX_SIZE);
  ssi2fStoreU32(nonce + SSI2F_NONCE_PREFIX_SIZE,
                (uint32_t)d->openssl_frame);
}

static BOOLEAN ssi2fWriteFrame(ssi2Info *d, const unsigned char *plain,
                               size_t plain_len, BOOLEAN final_frame)
{
  if (d->openssl_failed) return TRUE;
  if (d->openssl_frame>UINT32_MAX)
  {
    WerrorS("ssi2f: encrypted stream has too many frames");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  if (plain_len>SSI2F_PLAINTEXT_CHUNK_SIZE)
  {
    WerrorS("ssi2f: internal frame size error");
    d->openssl_failed=TRUE;
    return TRUE;
  }

  uint32_t frame_len=(uint32_t)plain_len;
  unsigned char prefix[4];
  unsigned char tag[SSI2F_TAG_SIZE];
  unsigned char ad[SSI2F_FRAME_AD_SIZE];
  unsigned char nonce[SSI2F_NONCE_SIZE];
  ssi2fStoreU32(prefix, frame_len | (final_frame ? 0x80000000U : 0));
  ssi2fFrameAssociatedData(d, frame_len, final_frame, ad);
  ssi2fFrameNonce(d, nonce);

  unsigned char *cipher=NULL;
  if (frame_len>0) cipher=(unsigned char*)omAlloc(frame_len);
  EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
  unsigned char final_buf[16];
  int out_len=0;
  int total_len=0;
  BOOLEAN failed=FALSE;
  const char *openssl_failure=NULL;
  ERR_clear_error();
  if (ctx==NULL)
    openssl_failure="create cipher context";
  else if (EVP_EncryptInit_ex(ctx, d->openssl_cipher, NULL, NULL, NULL)!=1)
    openssl_failure="initialize AES-256-GCM encryption";
  else if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
                               SSI2F_NONCE_SIZE, NULL)!=1)
    openssl_failure="set AES-256-GCM nonce length";
  else if (EVP_EncryptInit_ex(ctx, NULL, NULL, d->openssl_key, nonce)!=1)
    openssl_failure="set AES-256-GCM encryption key and nonce";
  else if (EVP_EncryptUpdate(ctx, NULL, &out_len, ad, sizeof(ad))!=1)
    openssl_failure="authenticate AES-256-GCM frame metadata";

  if (openssl_failure!=NULL)
  {
    failed=TRUE;
  }
  if ((!failed) && (frame_len>0))
  {
    out_len=0;
    if (EVP_EncryptUpdate(ctx, cipher, &out_len, plain, frame_len)!=1)
    {
      failed=TRUE;
      openssl_failure="encrypt AES-256-GCM frame payload";
    }
    total_len=out_len;
  }
  if (!failed)
  {
    out_len=0;
    if (EVP_EncryptFinal_ex(ctx, (frame_len>0) ? cipher+total_len : final_buf,
                            &out_len)!=1)
    {
      failed=TRUE;
      openssl_failure="finalize AES-256-GCM frame encryption";
    }
    else if (total_len+out_len!=(int)frame_len)
    {
      failed=TRUE;
      openssl_failure="validate AES-256-GCM encrypted frame length";
    }
    else if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG,
                                 SSI2F_TAG_SIZE, tag)!=1)
    {
      failed=TRUE;
      openssl_failure="get AES-256-GCM authentication tag";
    }
  }
  if (ctx!=NULL) EVP_CIPHER_CTX_free(ctx);

  if ((!failed)
  && ((fwrite(prefix, 1, sizeof(prefix), d->f_write)!=sizeof(prefix))
    || ((frame_len>0) && (fwrite(cipher, 1, frame_len, d->f_write)!=frame_len))
    || (fwrite(tag, 1, sizeof(tag), d->f_write)!=sizeof(tag))))
  {
    failed=TRUE;
  }
  if (cipher!=NULL)
  {
    OPENSSL_cleanse(cipher, frame_len);
    omFreeSize(cipher, frame_len);
  }
  OPENSSL_cleanse(tag, sizeof(tag));
  if (failed)
  {
    if (openssl_failure!=NULL)
      ssi2fReportOpenSSLError(openssl_failure);
    else
      WerrorS("ssi2f: encrypted write failed");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  d->openssl_frame++;
  return FALSE;
}

static BOOLEAN ssi2fReadFrame(ssi2Info *d)
{
  if (d->openssl_failed || d->openssl_final_seen) return TRUE;
  if (d->openssl_frame>UINT32_MAX)
  {
    WerrorS("ssi2f: encrypted stream has too many frames");
    d->openssl_failed=TRUE;
    return TRUE;
  }

  unsigned char prefix[4];
  if (fread(prefix, 1, sizeof(prefix), d->openssl_input)!=sizeof(prefix))
  {
    WerrorS("ssi2f: truncated encrypted stream (missing final frame)");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  uint32_t frame_info=ssi2fLoadU32(prefix);
  BOOLEAN final_frame=((frame_info & 0x80000000U)!=0);
  uint32_t frame_len=(frame_info & 0x7fffffffU);
  if (frame_len>SSI2F_PLAINTEXT_CHUNK_SIZE)
  {
    WerrorS("ssi2f: invalid encrypted frame length");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  if (final_frame && (frame_len!=0))
  {
    WerrorS("ssi2f: invalid encrypted final frame");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  if ((!final_frame) && (frame_len==0))
  {
    WerrorS("ssi2f: invalid empty encrypted frame");
    d->openssl_failed=TRUE;
    return TRUE;
  }

  unsigned char *cipher=NULL;
  if (frame_len>0) cipher=(unsigned char*)omAlloc(frame_len);
  if ((frame_len>0)
  && (fread(cipher, 1, frame_len, d->openssl_input)!=frame_len))
  {
    OPENSSL_cleanse(cipher, frame_len);
    omFreeSize(cipher, frame_len);
    WerrorS("ssi2f: truncated encrypted frame");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  unsigned char tag[SSI2F_TAG_SIZE];
  if (fread(tag, 1, sizeof(tag), d->openssl_input)!=sizeof(tag))
  {
    if (cipher!=NULL)
    {
      OPENSSL_cleanse(cipher, frame_len);
      omFreeSize(cipher, frame_len);
    }
    WerrorS("ssi2f: truncated encrypted frame tag");
    d->openssl_failed=TRUE;
    return TRUE;
  }

  unsigned char ad[SSI2F_FRAME_AD_SIZE];
  unsigned char nonce[SSI2F_NONCE_SIZE];
  ssi2fFrameAssociatedData(d, frame_len, final_frame, ad);
  ssi2fFrameNonce(d, nonce);

  EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
  unsigned char final_buf[16];
  int out_len=0;
  int total_len=0;
  BOOLEAN failed=FALSE;
  const char *openssl_failure=NULL;
  ERR_clear_error();
  if (ctx==NULL)
    openssl_failure="create cipher context";
  else if (EVP_DecryptInit_ex(ctx, d->openssl_cipher, NULL, NULL, NULL)!=1)
    openssl_failure="initialize AES-256-GCM decryption";
  else if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN,
                               SSI2F_NONCE_SIZE, NULL)!=1)
    openssl_failure="set AES-256-GCM nonce length";
  else if (EVP_DecryptInit_ex(ctx, NULL, NULL, d->openssl_key, nonce)!=1)
    openssl_failure="set AES-256-GCM decryption key and nonce";
  else if (EVP_DecryptUpdate(ctx, NULL, &out_len, ad, sizeof(ad))!=1)
    openssl_failure="authenticate AES-256-GCM frame metadata";

  if (openssl_failure!=NULL)
  {
    failed=TRUE;
  }
  if ((!failed) && (frame_len>0))
  {
    out_len=0;
    if (EVP_DecryptUpdate(ctx, d->openssl_read_buff, &out_len,
                          cipher, frame_len)!=1)
    {
      failed=TRUE;
      openssl_failure="decrypt AES-256-GCM frame payload";
    }
    total_len=out_len;
  }
  if (!failed)
  {
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG,
                            SSI2F_TAG_SIZE, tag)!=1)
    {
      failed=TRUE;
      openssl_failure="set AES-256-GCM authentication tag";
    }
    else
    {
      out_len=0;
      if (EVP_DecryptFinal_ex(ctx,
                              (frame_len>0) ? d->openssl_read_buff+total_len : final_buf,
                              &out_len)!=1)
      {
        failed=TRUE;
      }
      else if (total_len+out_len!=(int)frame_len)
      {
        failed=TRUE;
        openssl_failure="validate AES-256-GCM decrypted frame length";
      }
    }
  }
  if (ctx!=NULL) EVP_CIPHER_CTX_free(ctx);
  if (cipher!=NULL)
  {
    OPENSSL_cleanse(cipher, frame_len);
    omFreeSize(cipher, frame_len);
  }
  OPENSSL_cleanse(tag, sizeof(tag));
  if (failed)
  {
    if (openssl_failure!=NULL)
      ssi2fReportOpenSSLError(openssl_failure);
    else
      WerrorS("ssi2f: authentication failed (wrong key or modified data)");
    d->openssl_failed=TRUE;
    return TRUE;
  }
  d->openssl_frame++;

  if (final_frame)
  {
    int trailing=fgetc(d->openssl_input);
    if ((trailing!=EOF) || ferror(d->openssl_input))
    {
      WerrorS("ssi2f: trailing data after final frame");
      d->openssl_failed=TRUE;
      return TRUE;
    }
    d->openssl_final_seen=TRUE;
    d->openssl_read_pos=0;
    d->openssl_read_len=0;
    return FALSE;
  }
  d->openssl_read_pos=0;
  d->openssl_read_len=(size_t)frame_len;
  return FALSE;
}

static BOOLEAN ssi2fReadRaw(ssi2Info *d, void *buf, size_t len)
{
  unsigned char *p=(unsigned char*)buf;
  while (len>0)
  {
    if (d->openssl_read_pos<d->openssl_read_len)
    {
      size_t available=d->openssl_read_len-d->openssl_read_pos;
      size_t take=(len<available) ? len : available;
      memcpy(p, d->openssl_read_buff+d->openssl_read_pos, take);
      d->openssl_read_pos+=take;
      p+=take;
      len-=take;
      continue;
    }
    if (d->openssl_final_seen)
    {
      WerrorS("ssi2: unexpected end of input");
      return TRUE;
    }
    if (ssi2fReadFrame(d)) return TRUE;
  }
  return FALSE;
}

static BOOLEAN ssi2fDrain(ssi2Info *d)
{
  while ((!d->openssl_final_seen) && (!d->openssl_failed))
  {
    d->openssl_read_pos=d->openssl_read_len;
    if (ssi2fReadFrame(d)) break;
  }
  return d->openssl_failed;
}
#endif

static void ssi2FlushWriteBuffer(const ssiInfo *d)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd==NULL) || dd->write_failed || (dd->write_buff_pos<=0)) return;
  if (dd->f_write==NULL)
  {
    WerrorS("ssi2: output is not open");
    dd->write_failed=TRUE;
    return;
  }
#ifdef HAVE_LIBSODIUM
  if (dd->encrypted)
  {
    if (ssi2eWriteFrame(dd, (const unsigned char*)dd->write_buff,
                        (size_t)dd->write_buff_pos,
                        crypto_secretstream_xchacha20poly1305_TAG_MESSAGE))
      dd->write_failed=TRUE;
    dd->write_buff_pos=0;
    return;
  }
#endif
#ifdef HAVE_OPENSSL_FIPS
  if (dd->openssl_encrypted)
  {
    if (ssi2fWriteFrame(dd, (const unsigned char*)dd->write_buff,
                        (size_t)dd->write_buff_pos, FALSE))
      dd->write_failed=TRUE;
    dd->write_buff_pos=0;
    return;
  }
#endif
  if (fwrite(dd->write_buff, 1, dd->write_buff_pos, dd->f_write)!=(size_t)dd->write_buff_pos)
  {
    WerrorS("ssi2: write failed");
    dd->write_failed=TRUE;
  }
  dd->write_buff_pos=0;
}

static void ssi2FreeWriteBuffer(ssiInfo *d)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd!=NULL) && (dd->write_buff!=NULL))
  {
    ssi2FlushWriteBuffer(d);
#ifdef HAVE_LIBSODIUM
    if (dd->encrypted)
      sodium_memzero(dd->write_buff, dd->write_buff_size);
#endif
#ifdef HAVE_OPENSSL_FIPS
    if (dd->openssl_encrypted)
      OPENSSL_cleanse(dd->write_buff, dd->write_buff_size);
#endif
    omFreeSize(dd->write_buff, dd->write_buff_size);
    dd->write_buff=NULL;
    dd->write_buff_pos=0;
    dd->write_buff_size=0;
  }
}

static void ssi2WriteRaw(const ssiInfo *d, const void *buf, size_t len)
{
  if (len==0) return;
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd==NULL) || dd->write_failed) return;
  if (dd->write_buff==NULL)
  {
    dd->write_buff_size=1<<20;
    dd->write_buff=(char*)omAlloc(dd->write_buff_size);
    dd->write_buff_pos=0;
  }
#ifdef HAVE_LIBSODIUM
  if (dd->encrypted)
  {
    const unsigned char *p=(const unsigned char*)buf;
    while (len>0)
    {
      size_t available=(size_t)(dd->write_buff_size-dd->write_buff_pos);
      size_t take=(len<available) ? len : available;
      memcpy(dd->write_buff+dd->write_buff_pos, p, take);
      dd->write_buff_pos+=(int)take;
      p+=take;
      len-=take;
      if (dd->write_buff_pos==dd->write_buff_size)
        ssi2FlushWriteBuffer(d);
      if (dd->encryption_failed) return;
    }
    return;
  }
#endif
#ifdef HAVE_OPENSSL_FIPS
  if (dd->openssl_encrypted)
  {
    const unsigned char *p=(const unsigned char*)buf;
    while (len>0)
    {
      size_t available=(size_t)(dd->write_buff_size-dd->write_buff_pos);
      size_t take=(len<available) ? len : available;
      memcpy(dd->write_buff+dd->write_buff_pos, p, take);
      dd->write_buff_pos+=(int)take;
      p+=take;
      len-=take;
      if (dd->write_buff_pos==dd->write_buff_size)
        ssi2FlushWriteBuffer(d);
      if (dd->openssl_failed) return;
    }
    return;
  }
#endif
  if (len>=(size_t)dd->write_buff_size)
  {
    ssi2FlushWriteBuffer(d);
    if (dd->write_failed) return;
    if (fwrite(buf, 1, len, dd->f_write)!=len)
    {
      WerrorS("ssi2: write failed");
      dd->write_failed=TRUE;
    }
    return;
  }
  if (dd->write_buff_pos+(int)len>dd->write_buff_size)
    ssi2FlushWriteBuffer(d);
  if (dd->write_failed) return;
  memcpy(dd->write_buff+dd->write_buff_pos, buf, len);
  dd->write_buff_pos+=(int)len;
}

static void ssi2Fflush(const ssiInfo *d)
{
  ssi2FlushWriteBuffer(d);
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd!=NULL) && (!dd->write_failed) && (dd->f_write!=NULL))
  {
    if (fflush(dd->f_write)!=0)
    {
      WerrorS("ssi2: flush failed");
      dd->write_failed=TRUE;
    }
  }
}

static BOOLEAN ssi2ReadRaw(const ssiInfo *d, void *buf, size_t len)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd==NULL) || dd->read_failed) return TRUE;
#ifdef HAVE_LIBSODIUM
  if (dd->encrypted)
  {
    if (ssi2eReadRaw(dd, buf, len))
    {
      dd->read_failed=TRUE;
      return TRUE;
    }
    return FALSE;
  }
#endif
#ifdef HAVE_OPENSSL_FIPS
  if (dd->openssl_encrypted)
  {
    if (ssi2fReadRaw(dd, buf, len))
    {
      dd->read_failed=TRUE;
      return TRUE;
    }
    return FALSE;
  }
#endif
  if ((d->f_read==NULL) || (d->f_read->fd<0))
  {
    WerrorS("ssi2: input is not open");
    dd->read_failed=TRUE;
    return TRUE;
  }
  char *p=(char*)buf;
  while (len>0)
  {
    int chunk=(len>(size_t)INT_MAX) ? INT_MAX : (int)len;
    int got=s_readbytes(p, chunk, d->f_read);
    if (d->f_read->is_eof && (got>0)) got--;
    if (got!=chunk)
    {
      WerrorS("ssi2: unexpected end of input");
      dd->read_failed=TRUE;
      return TRUE;
    }
    p+=chunk;
    len-=chunk;
  }
  return FALSE;
}

static int ssi2ReadByte(const ssiInfo *d)
{
  unsigned char b=0;
  if (ssi2ReadRaw(d, &b, 1)) return -1;
  return (int)b;
}

static int ssi2ReadTopLevelByte(const ssiInfo *d)
{
  ssi2Info *dd=(ssi2Info*)d;
  if ((dd==NULL) || dd->read_failed) return -2;
#ifdef HAVE_LIBSODIUM
  if (dd->encrypted)
  {
    while (dd->encryption_read_pos>=dd->encryption_read_len)
    {
      if (dd->encryption_final_seen) return -1;
      if (ssi2eReadFrame(dd))
      {
        dd->read_failed=TRUE;
        return -2;
      }
    }
    return dd->encryption_read_buff[dd->encryption_read_pos++];
  }
#endif
#ifdef HAVE_OPENSSL_FIPS
  if (dd->openssl_encrypted)
  {
    while (dd->openssl_read_pos>=dd->openssl_read_len)
    {
      if (dd->openssl_final_seen) return -1;
      if (ssi2fReadFrame(dd))
      {
        dd->read_failed=TRUE;
        return -2;
      }
    }
    return dd->openssl_read_buff[dd->openssl_read_pos++];
  }
#endif
  if ((d->f_read==NULL) || (d->f_read->fd<0))
  {
    WerrorS("ssi2: input is not open");
    dd->read_failed=TRUE;
    return -2;
  }
  int c=s_getc(d->f_read);
  if (d->f_read->is_eof) return -1;
  return (int)(unsigned char)c;
}

static void ssi2WriteTag(const ssiInfo *d, unsigned char tag)
{
  ssi2WriteRaw(d, &tag, 1);
}

static int ssi2ReadTag(const ssiInfo *d)
{
  return ssi2ReadTopLevelByte(d);
}

static void ssi2WriteU64(const ssiInfo *d, uint64_t v)
{
  unsigned char buf[10];
  size_t len=0;
  do
  {
    unsigned char b=(unsigned char)(v & 0x7f);
    v >>= 7;
    if (v!=0) b |= 0x80;
    buf[len++]=b;
  }
  while (v!=0);
  ssi2WriteRaw(d, buf, len);
}

static uint64_t ssi2ReadU64(const ssiInfo *d)
{
  uint64_t v=0;
  for (int i=0; i<10; i++)
  {
    int c=ssi2ReadByte(d);
    if (c<0) return 0;
    if ((i==9) && (((c & 0x7f)>1) || ((c & 0x80)!=0)))
    {
      WerrorS("ssi2: integer is too large");
      ((ssi2Info*)d)->read_failed=TRUE;
      return 0;
    }
    v |= ((uint64_t)(c & 0x7f)) << (7*i);
    if ((c & 0x80)==0) return v;
  }
  WerrorS("ssi2: integer is too large");
  ((ssi2Info*)d)->read_failed=TRUE;
  return 0;
}

static uint64_t ssi2EncodeI64(int64_t v)
{
  return (((uint64_t)v) << 1) ^ (uint64_t)(v >> 63);
}

static int64_t ssi2DecodeI64(uint64_t v)
{
  return (int64_t)((v >> 1) ^ (uint64_t)(-(int64_t)(v & 1)));
}

static void ssi2WriteI64(const ssiInfo *d, int64_t v)
{
  ssi2WriteU64(d, ssi2EncodeI64(v));
}

static int64_t ssi2ReadI64(const ssiInfo *d)
{
  return ssi2DecodeI64(ssi2ReadU64(d));
}

static BOOLEAN ssi2ReadCount(const ssiInfo *d, uint64_t maximum,
                             const char *what, int *result)
{
  uint64_t value=ssi2ReadU64(d);
  if (((ssi2Info*)d)->read_failed) return TRUE;
  if ((value>maximum) || (value>(uint64_t)INT_MAX))
  {
    Werror("ssi2: %s is too large", what);
    ((ssi2Info*)d)->read_failed=TRUE;
    return TRUE;
  }
  *result=(int)value;
  return FALSE;
}

static BOOLEAN ssi2ReadInt(const ssiInfo *d, const char *what, int *result)
{
  int64_t value=ssi2ReadI64(d);
  if (((ssi2Info*)d)->read_failed) return TRUE;
  if ((value<INT_MIN) || (value>INT_MAX))
  {
    Werror("ssi2: %s is outside the supported range", what);
    ((ssi2Info*)d)->read_failed=TRUE;
    return TRUE;
  }
  *result=(int)value;
  return FALSE;
}

static BOOLEAN ssi2ReadLong(const ssiInfo *d, const char *what, long *result)
{
  int64_t value=ssi2ReadI64(d);
  if (((ssi2Info*)d)->read_failed) return TRUE;
#if LONG_MAX < INT64_MAX
  if ((value<LONG_MIN) || (value>LONG_MAX))
  {
    Werror("ssi2: %s is outside the supported range", what);
    ((ssi2Info*)d)->read_failed=TRUE;
    return TRUE;
  }
#endif
  *result=(long)value;
  return FALSE;
}

static BOOLEAN ssi2CheckedProduct(const ssiInfo *d, int a, int b,
                                  const char *what, int *result)
{
  const uint64_t max_entries=SIZE_MAX/sizeof(void*);
  if ((a<0) || (b<0) || ((a!=0) && (b>INT_MAX/a))
  || ((uint64_t)a*(uint64_t)b>max_entries))
  {
    Werror("ssi2: %s dimensions are too large", what);
    ((ssi2Info*)d)->read_failed=TRUE;
    return TRUE;
  }
  *result=a*b;
  return FALSE;
}

static void ssi2WriteSchemaTable(const ssiInfo *d)
{
  ssi2WriteTag(d, SSI_SCHEMA_TOKEN);
  ssi2WriteU64(d, SSI_SCHEMA_TABLE_VERSION);
  ssi2WriteU64(d, ssiSchemaVersionCount);
  for (int i=0; i<ssiSchemaVersionCount; i++)
  {
    int id=ssiSchemaVersions[i].id;
    ssi2WriteU64(d, id);
    ssi2WriteU64(d, ssiSchemaVersion(d, id));
  }
}

static BOOLEAN ssi2ReadSchemaTable(ssiInfo *d)
{
  ssi2Info *dd=(ssi2Info*)d;
  uint64_t table_version=ssi2ReadU64(d);
  uint64_t count=ssi2ReadU64(d);
  if (dd->read_failed) return TRUE;
  if (table_version!=SSI_SCHEMA_TABLE_VERSION)
  {
    Werror("ssi2: unsupported schema table version %llu (expected %d)",
           (unsigned long long)table_version, SSI_SCHEMA_TABLE_VERSION);
    dd->read_failed=TRUE;
    return TRUE;
  }
  if (count>1024)
  {
    WerrorS("ssi2: schema table has too many entries");
    dd->read_failed=TRUE;
    return TRUE;
  }
  memset(dd->schema_versions, 0, sizeof(dd->schema_versions));
  dd->validated_schema_versions=0;
  for (uint64_t i=0; i<count; i++)
  {
    uint64_t id=ssi2ReadU64(d);
    uint64_t version=ssi2ReadU64(d);
    if (dd->read_failed) return TRUE;
    if ((version==0) || (version>USHRT_MAX))
    {
      WerrorS("ssi2: invalid schema version in schema table");
      dd->read_failed=TRUE;
      return TRUE;
    }
    if ((id>0) && (id<SSI2_SCHEMA_VERSION_COUNT))
    {
      if (dd->schema_versions[id]!=0)
      {
        Werror("ssi2: duplicate schema entry for type %llu",
               (unsigned long long)id);
        dd->read_failed=TRUE;
        return TRUE;
      }
      ssiSetSchemaVersion(d, (int)id, (int)version);
    }
  }
  dd->schema_table_seen=TRUE;
  return FALSE;
}

static void ssi2WriteString(const ssiInfo *d, const char *s)
{
  size_t l=strlen(s);
  ssi2WriteU64(d, (uint64_t)l);
  ssi2WriteRaw(d, s, l);
}

static char *ssi2ReadString(const ssiInfo *d)
{
  uint64_t l64=ssi2ReadU64(d);
  if (((ssi2Info*)d)->read_failed) return NULL;
  if (l64>(uint64_t)INT_MAX)
  {
    WerrorS("ssi2: string too large");
    ((ssi2Info*)d)->read_failed=TRUE;
    return NULL;
  }
  size_t l=(size_t)l64;
  char *buf=(char*)omAlloc0(l+1);
  if (ssi2ReadRaw(d, buf, l))
  {
    omFree(buf);
    return NULL;
  }
  buf[l]='\0';
  return buf;
}

static void ssi2WriteMpz(const ssiInfo *d, const mpz_t z)
{
  int sign=mpz_sgn(z);
  ssi2WriteI64(d, sign);
  if (sign==0)
  {
    ssi2WriteU64(d, 0);
    return;
  }
  size_t len=(mpz_sizeinbase(z, 2)+7)/8;
  char *buf=(char*)omAlloc(len);
  size_t written=0;
  mpz_export(buf, &written, 1, 1, 1, 0, z);
  ssi2WriteU64(d, (uint64_t)written);
  ssi2WriteRaw(d, buf, written);
  omFreeSize(buf, len);
}

static void ssi2WriteLongAsMpz(const ssiInfo *d, long v)
{
  int sign=(v>0) - (v<0);
  ssi2WriteI64(d, sign);
  if (sign==0)
  {
    ssi2WriteU64(d, 0);
    return;
  }

  unsigned long a=(sign<0) ? (0UL - (unsigned long)v) : (unsigned long)v;
  unsigned char buf[sizeof(unsigned long)];
  size_t len=0;
  while (a!=0)
  {
    buf[sizeof(buf)-1-len]=(unsigned char)(a & 0xff);
    a >>= 8;
    len++;
  }
  ssi2WriteU64(d, len);
  ssi2WriteRaw(d, buf+sizeof(buf)-len, len);
}

static void ssi2ReadMpz(const ssiInfo *d, mpz_t z)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MPZ, "ssi2"))
  {
    mpz_set_ui(z, 0);
    return;
  }
  int sign=0;
  if (ssi2ReadInt(d, "mpz sign", &sign))
  {
    mpz_set_ui(z, 0);
    return;
  }
  if ((sign<-1) || (sign>1))
  {
    WerrorS("ssi2: invalid mpz sign");
    ((ssi2Info*)d)->read_failed=TRUE;
    mpz_set_ui(z, 0);
    return;
  }
  uint64_t len64=ssi2ReadU64(d);
  if (((ssi2Info*)d)->read_failed)
  {
    mpz_set_ui(z, 0);
    return;
  }
  if (len64>(uint64_t)INT_MAX)
  {
    WerrorS("ssi2: mpz payload too large");
    ((ssi2Info*)d)->read_failed=TRUE;
    mpz_set_ui(z, 0);
    return;
  }
  size_t len=(size_t)len64;
  if (len==0)
  {
    if (sign!=0)
    {
      WerrorS("ssi2: invalid empty mpz payload");
      ((ssi2Info*)d)->read_failed=TRUE;
    }
    mpz_set_ui(z, 0);
    return;
  }
  if (sign==0)
  {
    WerrorS("ssi2: invalid zero mpz sign");
    ((ssi2Info*)d)->read_failed=TRUE;
    mpz_set_ui(z, 0);
    return;
  }
  char *buf=(char*)omAlloc(len);
  if (ssi2ReadRaw(d, buf, len))
  {
    omFreeSize(buf, len);
    mpz_set_ui(z, 0);
    return;
  }
  mpz_import(z, len, 1, 1, 1, 0, buf);
  if (sign<0) mpz_neg(z, z);
  omFreeSize(buf, len);
}

static void ssi2WriteNumberAsMpz(const ssiInfo *d, number n, const coeffs cf)
{
  mpz_t z;
  number tmp=n;
  n_MPZ(z, tmp, cf);
  ssi2WriteMpz(d, z);
  mpz_clear(z);
}

static void ssi2WriteQIntegerNumber(const ssiInfo *d, number n, const coeffs cf)
{
  number tmp=n;
  n_Normalize(tmp, cf);
  if (SR_HDL(tmp) & SR_INT)
    ssi2WriteLongAsMpz(d, SR_TO_INT(tmp));
  else
    ssi2WriteMpz(d, tmp->z);
}

static void ssi2WriteQQNumber(const ssiInfo *d, number n, const coeffs cf)
{
  number tmp=n;
  n_Normalize(tmp, cf);
  if (SR_HDL(tmp) & SR_INT)
  {
    ssi2WriteLongAsMpz(d, SR_TO_INT(tmp));
    ssi2WriteLongAsMpz(d, 1);
  }
  else
  {
    ssi2WriteMpz(d, tmp->z);
    if (tmp->s==3)
      ssi2WriteLongAsMpz(d, 1);
    else
      ssi2WriteMpz(d, tmp->n);
  }
}

static void ssi2WriteNumber_CF(const ssiInfo *d, number n, const coeffs cf)
{
  if (((ssi2Info*)d)->write_failed) return;
  switch (getCoeffType(cf))
  {
    case n_transExt:
    {
      fraction f=(fraction)n;
      ssi2WritePoly_R(d, NUM(f), cf->extRing);
      ssi2WritePoly_R(d, DEN(f), cf->extRing);
      break;
    }
    case n_algExt:
      ssi2WritePoly_R(d, (poly)n, cf->extRing);
      break;
    case n_Q:
      if (cf->is_field)
      {
        ssi2WriteQQNumber(d, n, cf);
      }
      else
      {
        ssi2WriteQIntegerNumber(d, n, cf);
      }
      break;
    case n_Z:
    case n_Zn:
    case n_Znm:
    case n_Z2m:
      {
        ssi2WriteNumberAsMpz(d, n, cf);
      }
      break;
    case n_Zp:
    case n_GF:
      {
        number tmp=n;
        ssi2WriteI64(d, n_Int(tmp, cf));
      }
      break;
    default:
      Werror("ssi2: coeff type %d not implemented", (int)getCoeffType(cf));
      ((ssi2Info*)d)->write_failed=TRUE;
      break;
  }
}

static number ssi2ReadNumber_CF(const ssiInfo *d, const coeffs cf)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_NUMBER, "ssi2")) return NULL;
  switch (getCoeffType(cf))
  {
    case n_transExt:
    {
      fraction f=(fraction)n_Init(1, cf);
      p_Delete(&NUM(f), cf->extRing);
      NUM(f)=ssi2ReadPoly_R(d, cf->extRing);
      DEN(f)=ssi2ReadPoly_R(d, cf->extRing);
      if (((ssi2Info*)d)->read_failed || (DEN(f)==NULL))
      {
        if (!((ssi2Info*)d)->read_failed)
        {
          WerrorS("ssi2: extension-field denominator is zero");
          ((ssi2Info*)d)->read_failed=TRUE;
        }
        number failed_fraction=(number)f;
        n_Delete(&failed_fraction, cf);
        return NULL;
      }
      return (number)f;
    }
    case n_algExt:
      return (number)ssi2ReadPoly_R(d, cf->extRing);
    case n_Q:
      if (cf->is_field)
      {
        mpz_t num;
        mpz_t den;
        mpz_init(num);
        mpz_init(den);
        ssi2ReadMpz(d, num);
        ssi2ReadMpz(d, den);
        if (((ssi2Info*)d)->read_failed || (mpz_sgn(den)==0))
        {
          if (!((ssi2Info*)d)->read_failed)
          {
            WerrorS("ssi2: rational denominator is zero");
            ((ssi2Info*)d)->read_failed=TRUE;
          }
          mpz_clear(den);
          mpz_clear(num);
          return NULL;
        }
        number n=n_InitMPZ(num, cf);
        number dnum=n_InitMPZ(den, cf);
        number res=n_Div(n, dnum, cf);
        n_Delete(&n, cf);
        n_Delete(&dnum, cf);
        mpz_clear(den);
        mpz_clear(num);
        return res;
      }
      else
      {
        mpz_t z;
        mpz_init(z);
        ssi2ReadMpz(d, z);
        if (((ssi2Info*)d)->read_failed)
        {
          mpz_clear(z);
          return NULL;
        }
        number res=n_InitMPZ(z, cf);
        mpz_clear(z);
        return res;
      }
    case n_Z:
    case n_Zn:
    case n_Znm:
    case n_Z2m:
      {
        mpz_t z;
        mpz_init(z);
        ssi2ReadMpz(d, z);
        if (((ssi2Info*)d)->read_failed)
        {
          mpz_clear(z);
          return NULL;
        }
        number res=n_InitMPZ(z, cf);
        mpz_clear(z);
        return res;
      }
    case n_Zp:
    case n_GF:
      {
        long value=0;
        if (ssi2ReadLong(d, "coefficient", &value)) return NULL;
        return n_Init(value, cf);
      }
    default:
      Werror("ssi2: coeff type %d not implemented", (int)getCoeffType(cf));
      ((ssi2Info*)d)->read_failed=TRUE;
      return NULL;
  }
}

static void ssi2WriteNumber(const ssiInfo *d, number n)
{
  ssi2WriteNumber_CF(d, n, d->r->cf);
}

static number ssi2ReadNumber(ssiInfo *d)
{
  return ssi2ReadNumber_CF(d, d->r->cf);
}

static void ssi2WriteBigInt(const ssiInfo *d, number n)
{
  ssi2WriteQIntegerNumber(d, n, coeffs_BIGINT);
}

static number ssi2ReadBigInt(const ssiInfo *d)
{
  mpz_t z;
  mpz_init(z);
  ssi2ReadMpz(d, z);
  if (((ssi2Info*)d)->read_failed)
  {
    mpz_clear(z);
    return NULL;
  }
  number res=n_InitMPZ(z, coeffs_BIGINT);
  mpz_clear(z);
  return res;
}

static void ssi2WriteRing_R(ssiInfo *d, const ring r)
{
  if (r!=NULL)
  {
    for (int i=0; i<SI_RING_CACHE; i++)
    {
      if (d->rings[i]==r)
      {
        ssi2WriteI64(d, -5);
        ssi2WriteU64(d, i);
        return;
      }
    }
    for (int i=0; i<SI_RING_CACHE; i++)
    {
      if (d->rings[i]==NULL)
      {
        d->rings[i]=rIncRefCnt(r);
        ssi2WriteI64(d, -6);
        ssi2WriteU64(d, i);
        break;
      }
    }
    if (rField_is_Q(r) || rField_is_Zp(r))
      ssi2WriteI64(d, n_GetChar(r->cf));
    else if (rFieldType(r)==n_transExt)
      ssi2WriteI64(d, -1);
    else if (rFieldType(r)==n_algExt)
      ssi2WriteI64(d, -2);
    else
    {
      ssi2WriteI64(d, -3);
    }
    ssi2WriteU64(d, r->N);
    if ((!rField_is_Q(r)) && (!rField_is_Zp(r))
    && (rFieldType(r)!=n_transExt) && (rFieldType(r)!=n_algExt))
      ssi2WriteString(d, nCoeffName(r->cf));
    for (int i=0; i<r->N; i++)
      ssi2WriteString(d, r->names[i]);
    int n_ord=0;
    if (r->order!=NULL) while (r->order[n_ord]!=0) n_ord++;
    ssi2WriteU64(d, n_ord);
    for (int i=0; i<n_ord; i++)
    {
      ssi2WriteI64(d, r->order[i]);
      ssi2WriteI64(d, r->block0[i]);
      ssi2WriteI64(d, r->block1[i]);
      switch (r->order[i])
      {
        case ringorder_a:
        case ringorder_wp:
        case ringorder_Wp:
        case ringorder_ws:
        case ringorder_Ws:
        case ringorder_aa:
        {
          int s=r->block1[i]-r->block0[i]+1;
          for (int j=0; j<s; j++) ssi2WriteI64(d, r->wvhdl[i][j]);
          break;
        }
        case ringorder_M:
        {
          int s=r->block1[i]-r->block0[i]+1;
          for (int j=0; j<s*s; j++) ssi2WriteI64(d, r->wvhdl[i][j]);
          break;
        }
        case ringorder_a64:
        case ringorder_L:
        case ringorder_IS:
          Werror("ring order not implemented for ssi2:%d", r->order[i]);
          ((ssi2Info*)d)->write_failed=TRUE;
          break;
        default:
          break;
      }
    }
    if ((rFieldType(r)==n_transExt) || (rFieldType(r)==n_algExt))
      ssi2WriteRing_R(d, r->cf->extRing);
    if (r->qideal!=NULL)
      ssi2WriteIdeal_R(d, IDEAL_CMD, r->qideal, r);
    else
      ssi2WriteU64(d, 0);
  }
  else
  {
    ssi2WriteI64(d, 0);
    ssi2WriteU64(d, 0);
    ssi2WriteU64(d, 0);
    ssi2WriteU64(d, 0);
  }
  if ((r!=NULL) && rIsLPRing(r))
  {
    ssi2WriteTag(d, 23);
    ssi2WriteU64(d, 1);
    ssi2WriteU64(d, SI_LOG2(r->bitmask));
    ssi2WriteI64(d, r->isLPring);
  }
  else if (r!=NULL)
  {
    unsigned long bm=0;
    int b=0;
    bm=rGetExpSize(bm, b, r->N);
    if (r->bitmask!=bm)
    {
      ssi2WriteTag(d, 23);
      ssi2WriteU64(d, 0);
      ssi2WriteU64(d, SI_LOG2(r->bitmask));
    }
    if (rIsPluralRing(r))
    {
      ssi2WriteTag(d, 23);
      ssi2WriteU64(d, 2);
      ssi2WriteIdeal_R(d, MATRIX_CMD, (ideal)r->GetNC()->C, r);
      ssi2WriteIdeal_R(d, MATRIX_CMD, (ideal)r->GetNC()->D, r);
    }
  }
}

static void ssi2WriteRing(ssiInfo *d, const ring r)
{
  if ((r==NULL) || (r->cf==NULL))
  {
    ssi2WriteI64(d, -4);
    return;
  }
  if (r==currRing)
  {
    if (d->r!=NULL) rKill(d->r);
    d->r=r;
  }
  rIncRefCnt(r);
  ssi2WriteRing_R(d, r);
}

static ring ssi2ReadRing(ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING, "ssi2")) return NULL;
  ssi2Info *dd=(ssi2Info*)d;
  int ch=0;
  if (ssi2ReadInt(d, "coefficient type", &ch)) return NULL;
  int new_ref=-1;
  if (ch==-6)
  {
    if (ssi2ReadCount(d, SI_RING_CACHE-1, "ring cache index", &new_ref))
      return NULL;
    if (d->rings[new_ref]!=NULL)
    {
      Werror("ssi2: ring cache slot %d is already occupied", new_ref);
      dd->read_failed=TRUE;
      return NULL;
    }
    if (ssi2ReadInt(d, "coefficient type", &ch)) return NULL;
    if ((ch==-6) || (ch==-5) || (ch==-4))
    {
      WerrorS("ssi2: invalid nested ring cache marker");
      dd->read_failed=TRUE;
      return NULL;
    }
  }
  if (ch==-5)
  {
    int index=0;
    if (ssi2ReadCount(d, SI_RING_CACHE-1, "ring cache index", &index))
      return NULL;
    ring r=d->rings[index];
    if (r==NULL)
    {
      Werror("ssi2: ring cache reference %d is undefined", index);
      dd->read_failed=TRUE;
      return NULL;
    }
    rIncRefCnt(r);
    return r;
  }
  if (ch==-4) return NULL;
  int N=0;
  uint64_t max_names=(uint64_t)(SIZE_MAX/sizeof(char*));
  if (ssi2ReadCount(d, max_names, "ring variable count", &N)) return NULL;
  char **names=NULL;
  coeffs cf=NULL;
  if (ch==-3)
  {
    char *cf_name=ssi2ReadString(d);
    if (cf_name==NULL) return NULL;
    cf=nFindCoeffByName(cf_name);
    if (cf==NULL)
    {
      Werror("cannot find cf:%s", cf_name);
      dd->read_failed=TRUE;
      omFree(cf_name);
      return NULL;
    }
    omFree(cf_name);
  }
  if (N!=0)
  {
    names=(char**)omAlloc0((size_t)N*sizeof(char*));
    for (int i=0; i<N; i++)
    {
      names[i]=ssi2ReadString(d);
      if (names[i]==NULL)
      {
        for (int j=0; j<i; j++) omFree(names[j]);
        omFreeSize(names, (size_t)N*sizeof(char*));
        return NULL;
      }
    }
  }
  int num_ord=0;
  uint64_t max_orders=(uint64_t)(SIZE_MAX/sizeof(int*))-1;
  if (max_orders>(uint64_t)INT_MAX-1) max_orders=(uint64_t)INT_MAX-1;
  if (ssi2ReadCount(d, max_orders, "ring ordering count", &num_ord))
  {
    for (int i=0; i<N; i++) omFree(names[i]);
    if (names!=NULL) omFreeSize(names, (size_t)N*sizeof(char*));
    return NULL;
  }
  size_t order_slots=(size_t)num_ord+1;
  rRingOrder_t *ord=(rRingOrder_t*)omAlloc0(order_slots*sizeof(rRingOrder_t));
  int *block0=(int*)omAlloc0(order_slots*sizeof(int));
  int *block1=(int*)omAlloc0(order_slots*sizeof(int));
  int **wvhdl=(int**)omAlloc0(order_slots*sizeof(int*));
  for (int i=0; i<num_ord; i++)
  {
    int order_value=0;
    if (ssi2ReadInt(d, "ring ordering", &order_value)
    || ssi2ReadInt(d, "ring ordering block start", &block0[i])
    || ssi2ReadInt(d, "ring ordering block end", &block1[i]))
      break;
    if ((order_value<=ringorder_no) || (order_value>=ringorder_unspec))
    {
      Werror("ssi2: invalid ring ordering %d", order_value);
      dd->read_failed=TRUE;
      break;
    }
    ord[i]=(rRingOrder_t)order_value;
    switch (ord[i])
    {
      case ringorder_a:
      case ringorder_wp:
      case ringorder_Wp:
      case ringorder_ws:
      case ringorder_Ws:
      case ringorder_aa:
      {
        if ((block0[i]<1) || (block1[i]<block0[i]) || (block1[i]>N))
        {
          WerrorS("ssi2: invalid weighted ordering block");
          dd->read_failed=TRUE;
          break;
        }
        int s=block1[i]-block0[i]+1;
        wvhdl[i]=(int*)omAlloc((size_t)s*sizeof(int));
        for (int j=0; j<s; j++)
          if (ssi2ReadInt(d, "ring ordering weight", &wvhdl[i][j])) break;
        break;
      }
      case ringorder_M:
      {
        if ((block0[i]<1) || (block1[i]<block0[i]) || (block1[i]>N))
        {
          WerrorS("ssi2: invalid matrix ordering block");
          dd->read_failed=TRUE;
          break;
        }
        int s=block1[i]-block0[i]+1;
        int entries=0;
        if (ssi2CheckedProduct(d, s, s, "matrix ordering", &entries)) break;
        wvhdl[i]=(int*)omAlloc((size_t)entries*sizeof(int));
        for (int j=0; j<entries; j++)
          if (ssi2ReadInt(d, "ring ordering weight", &wvhdl[i][j])) break;
        break;
      }
      case ringorder_a64:
      case ringorder_L:
      case ringorder_IS:
        Werror("ring order not implemented for ssi2:%d", ord[i]);
        dd->read_failed=TRUE;
        break;
      default:
        break;
    }
    if (dd->read_failed) break;
  }
  if (dd->read_failed)
  {
    for (int i=0; i<N; i++) if (names[i]!=NULL) omFree(names[i]);
    if (names!=NULL) omFreeSize(names, (size_t)N*sizeof(char*));
    for (int i=0; i<num_ord; i++) if (wvhdl[i]!=NULL) omFree(wvhdl[i]);
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  if (N==0)
  {
    WerrorS("ssi2: ring has no variables");
    dd->read_failed=TRUE;
    omFree(ord);
    omFree(block0);
    omFree(block1);
    omFree(wvhdl);
    return NULL;
  }
  ring r=NULL;
  if (ch>=0)
    r=rDefault(ch, N, names, num_ord, ord, block0, block1, wvhdl);
  else if (ch==-1)
  {
    TransExtInfo T;
    T.r=ssi2ReadRing(d);
    if (T.r==NULL)
    {
      dd->read_failed=TRUE;
      return NULL;
    }
    cf=nInitChar(n_transExt, &T);
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  }
  else if (ch==-2)
  {
    TransExtInfo T;
    T.r=ssi2ReadRing(d);
    if (T.r==NULL)
    {
      dd->read_failed=TRUE;
      return NULL;
    }
    cf=nInitChar(n_algExt, &T);
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  }
  else if (ch==-3)
    r=rDefault(cf, N, names, num_ord, ord, block0, block1, wvhdl);
  else
  {
    Werror("ssi2: read unknown coeffs type (%d)", ch);
    dd->read_failed=TRUE;
    for (int i=0; i<N; i++) omFree(names[i]);
    omFreeSize(names, N*sizeof(char*));
    return NULL;
  }
  if (r==NULL)
  {
    WerrorS("ssi2: could not construct ring");
    dd->read_failed=TRUE;
    for (int i=0; i<N; i++) omFree(names[i]);
    omFreeSize(names, (size_t)N*sizeof(char*));
    return NULL;
  }
  ideal q=ssi2ReadIdeal_R(d, r);
  if ((q==NULL) || dd->read_failed)
  {
    rDelete(r);
    return NULL;
  }
  if (IDELEMS(q)==0) omFreeBin(q, sip_sideal_bin);
  else r->qideal=q;
  for (int i=0; i<N; i++) omFree(names[i]);
  omFreeSize(names, N*sizeof(char*));
  rIncRefCnt(r);
  char name[20];
  int nr=0;
  idhdl h=NULL;
  loop
  {
    snprintf(name, 20, "ssiRing%d", nr); nr++;
    h=IDROOT->get(name, 0);
    if (h==NULL) break;
    else if ((IDTYP(h)==RING_CMD) && (r!=IDRING(h)) && (rEqual(r, IDRING(h), 1)))
    {
      rDelete(r);
      r=rIncRefCnt(IDRING(h));
      break;
    }
  }
  if (new_ref!=-1)
  {
    d->rings[new_ref]=r;
    rIncRefCnt(r);
  }
  return r;
}

static void ssi2WritePoly_R(const ssiInfo *d, poly p, const ring r)
{
  ssi2WriteU64(d, pLength(p));
  while (p!=NULL)
  {
    ssi2WriteNumber_CF(d, pGetCoeff(p), r->cf);
    ssi2WriteI64(d, p_GetComp(p, r));
    for (int j=1; j<=rVar(r); j++)
      ssi2WriteI64(d, p_GetExp(p, j, r));
    pIter(p);
  }
}

static void ssi2WritePoly(const ssiInfo *d, poly p)
{
  ssi2WritePoly_R(d, p, d->r);
}

static poly ssi2ReadPoly_R(const ssiInfo *d, const ring r)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_POLY, "ssi2")) return NULL;
  int n=0;
  if (ssi2ReadCount(d, INT_MAX, "polynomial term count", &n)) return NULL;
  poly ret=NULL;
  poly prev=NULL;
  for (int l=0; l<n; l++)
  {
    poly p=p_Init(r, r->PolyBin);
    pSetCoeff0(p, ssi2ReadNumber_CF(d, r->cf));
    int component=0;
    if (((ssi2Info*)d)->read_failed
    || ssi2ReadInt(d, "polynomial component", &component)
    || (component<0))
    {
      if (!((ssi2Info*)d)->read_failed)
      {
        WerrorS("ssi2: invalid polynomial component");
        ((ssi2Info*)d)->read_failed=TRUE;
      }
      p_Delete(&p, r);
      p_Delete(&ret, r);
      return NULL;
    }
    p_SetComp(p, component, r);
    for (int i=1; i<=rVar(r); i++)
    {
      long exponent=0;
      if (ssi2ReadLong(d, "polynomial exponent", &exponent)
      || (exponent<0) || ((unsigned long)exponent>r->bitmask))
      {
        if (!((ssi2Info*)d)->read_failed)
        {
          WerrorS("ssi2: invalid polynomial exponent");
          ((ssi2Info*)d)->read_failed=TRUE;
        }
        p_Delete(&p, r);
        p_Delete(&ret, r);
        return NULL;
      }
      p_SetExp(p, i, exponent, r);
    }
    p_Setm(p, r);
    p_Test(p, r);
    if (ret==NULL) ret=p;
    else pNext(prev)=p;
    prev=p;
  }
  return ret;
}

static poly ssi2ReadPoly(ssiInfo *d)
{
  return ssi2ReadPoly_R(d, d->r);
}

static void ssi2WriteIdeal_R(const ssiInfo *d, int typ, const ideal I, const ring R)
{
  matrix M=(matrix)I;
  int mn;
  if (typ==MATRIX_CMD)
  {
    mn=MATROWS(M)*MATCOLS(M);
    ssi2WriteU64(d, MATROWS(M));
    ssi2WriteU64(d, MATCOLS(M));
  }
  else
  {
    mn=IDELEMS(I);
    ssi2WriteU64(d, IDELEMS(I));
  }
  for (int i=0; i<mn; i++) ssi2WritePoly_R(d, I->m[i], R);
}

static void ssi2WriteIdeal(const ssiInfo *d, int typ, const ideal I)
{
  ssi2WriteIdeal_R(d, typ, I, d->r);
}

static ideal ssi2ReadIdeal_R(const ssiInfo *d, const ring r)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_IDEAL, "ssi2")) return NULL;
  int n=0;
  uint64_t maximum=(uint64_t)(SIZE_MAX/sizeof(poly));
  if (ssi2ReadCount(d, maximum, "ideal generator count", &n)) return NULL;
  ideal I=idInit(n, 1);
  for (int i=0; i<IDELEMS(I); i++)
  {
    I->m[i]=ssi2ReadPoly_R(d, r);
    if (((ssi2Info*)d)->read_failed)
    {
      id_Delete(&I, r);
      return NULL;
    }
  }
  return I;
}

static ideal ssi2ReadIdeal(ssiInfo *d)
{
  return ssi2ReadIdeal_R(d, d->r);
}

static matrix ssi2ReadMatrix(ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MATRIX, "ssi2")) return NULL;
  int m=0;
  int n=0;
  int entries=0;
  if (ssi2ReadCount(d, INT_MAX, "matrix row count", &m)
  || ssi2ReadCount(d, INT_MAX, "matrix column count", &n)
  || ssi2CheckedProduct(d, m, n, "matrix", &entries)) return NULL;
  matrix M=mpNew(m, n);
  for (int i=1; i<=MATROWS(M); i++)
    for (int j=1; j<=MATCOLS(M); j++)
    {
      MATELEM(M, i, j)=ssi2ReadPoly(d);
      if (((ssi2Info*)d)->read_failed)
      {
        id_Delete((ideal*)&M, d->r);
        return NULL;
      }
    }
  return M;
}

static void ssi2WriteCommand(si_link l, command D)
{
  ssiInfo *d=(ssiInfo*)l->data;
  ssi2WriteU64(d, D->argc);
  ssi2WriteI64(d, D->op);
  if (D->argc>0) ssi2Write(l, &(D->arg1));
  if (D->argc<4)
  {
    if (D->argc>1) ssi2Write(l, &(D->arg2));
    if (D->argc>2) ssi2Write(l, &(D->arg3));
  }
}

static command ssi2ReadCommand(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_COMMAND, "ssi2")) return NULL;
  command D=(command)omAlloc0(sizeof(*D));
  int argc=0;
  int op=0;
  if (ssi2ReadCount(d, SHRT_MAX, "command argument count", &argc)
  || ssi2ReadInt(d, "command operation", &op)
  || (op<SHRT_MIN) || (op>SHRT_MAX))
  {
    if (!((ssi2Info*)d)->read_failed)
    {
      WerrorS("ssi2: command operation is outside the supported range");
      ((ssi2Info*)d)->read_failed=TRUE;
    }
    omFree(D);
    return NULL;
  }
  D->argc=argc;
  D->op=op;
  leftv v;
  if (argc>0)
  {
    v=ssi2Read1(l);
    if (v==NULL)
    {
      omFree(D);
      return NULL;
    }
    memcpy(&(D->arg1), v, sizeof(*v));
    omFreeBin(v, sleftv_bin);
  }
  if (argc<4)
  {
    if (D->argc>1)
    {
      v=ssi2Read1(l);
      if (v==NULL)
      {
        D->arg1.CleanUp(d->r);
        omFree(D);
        return NULL;
      }
      memcpy(&(D->arg2), v, sizeof(*v));
      omFreeBin(v, sleftv_bin);
    }
    if (D->argc>2)
    {
      v=ssi2Read1(l);
      if (v==NULL)
      {
        D->arg1.CleanUp(d->r);
        D->arg2.CleanUp(d->r);
        omFree(D);
        return NULL;
      }
      memcpy(&(D->arg3), v, sizeof(*v));
      omFreeBin(v, sleftv_bin);
    }
  }
  else
  {
    leftv prev=&(D->arg1);
    argc--;
    while (argc>0)
    {
      v=ssi2Read1(l);
      if (v==NULL)
      {
        D->arg1.CleanUp(d->r);
        D->arg2.CleanUp(d->r);
        D->arg3.CleanUp(d->r);
        omFree(D);
        return NULL;
      }
      prev->next=v;
      prev=v;
      argc--;
    }
  }
  return D;
}

static void ssi2WriteProc(const ssiInfo *d, procinfov p)
{
  if (p->data.s.body==NULL) iiGetLibProcBuffer(p);
  if (p->data.s.body!=NULL) ssi2WriteString(d, p->data.s.body);
  else ssi2WriteString(d, "");
}

static procinfov ssi2ReadProc(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_PROC, "ssi2")) return NULL;
  char *s=ssi2ReadString(d);
  if (s==NULL) return NULL;
  procinfov p=(procinfov)omAlloc0Bin(procinfo_bin);
  p->language=LANG_SINGULAR;
  p->libname=omStrDup("");
  p->procname=omStrDup("");
  p->data.s.body=s;
  return p;
}

static void ssi2WriteList(si_link l, lists dd)
{
  ssiInfo *d=(ssiInfo*)l->data;
  int Ll=dd->nr;
  ssi2WriteU64(d, Ll+1);
  for (int i=0; i<=Ll; i++) ssi2Write(l, &(dd->m[i]));
}

static lists ssi2ReadList(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_LIST, "ssi2")) return NULL;
  int nr=0;
  uint64_t maximum=(uint64_t)(SIZE_MAX/sizeof(sleftv));
  if (ssi2ReadCount(d, maximum, "list length", &nr)) return NULL;
  lists L=(lists)omAlloc0Bin(slists_bin);
  L->Init(nr);
  for (int i=0; i<=L->nr; i++)
  {
    leftv v=ssi2Read1(l);
    if (v==NULL)
    {
      L->Clean(d->r);
      return NULL;
    }
    memcpy(&(L->m[i]), v, sizeof(*v));
    omFreeBin(v, sleftv_bin);
  }
  return L;
}

static void ssi2WriteIntvec(const ssiInfo *d, intvec *v)
{
  ssi2WriteU64(d, v->length());
  for (int i=0; i<v->length(); i++) ssi2WriteI64(d, (*v)[i]);
}

static intvec *ssi2ReadIntvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi2")) return NULL;
  int nr=0;
  if (ssi2ReadCount(d, SIZE_MAX/sizeof(int), "intvec length", &nr)) return NULL;
  intvec *v=new intvec(nr);
  for (int i=0; i<nr; i++)
  {
    if (ssi2ReadInt(d, "intvec entry", &(*v)[i]))
    {
      delete v;
      return NULL;
    }
  }
  return v;
}

static void ssi2WriteIntmat(const ssiInfo *d, intvec *v)
{
  ssi2WriteU64(d, v->rows());
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteI64(d, (*v)[i]);
}

static intvec *ssi2ReadIntmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_INTVEC, "ssi2")) return NULL;
  int r=0;
  int c=0;
  int entries=0;
  if (ssi2ReadCount(d, INT_MAX, "intmat row count", &r)
  || ssi2ReadCount(d, INT_MAX, "intmat column count", &c)
  || ssi2CheckedProduct(d, r, c, "intmat", &entries)) return NULL;
  intvec *v=new intvec(r, c, 0);
  for (int i=0; i<entries; i++)
  {
    if (ssi2ReadInt(d, "intmat entry", &(*v)[i]))
    {
      delete v;
      return NULL;
    }
  }
  return v;
}

static void ssi2WriteBigintmat(const ssiInfo *d, bigintmat *v)
{
  ssi2WriteU64(d, v->rows());
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteBigInt(d, (*v)[i]);
}

static bigintmat *ssi2ReadBigintmat(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi2")) return NULL;
  int r=0;
  int c=0;
  int entries=0;
  if (ssi2ReadCount(d, INT_MAX, "bigintmat row count", &r)
  || ssi2ReadCount(d, INT_MAX, "bigintmat column count", &c)
  || ssi2CheckedProduct(d, r, c, "bigintmat", &entries)) return NULL;
  bigintmat *v=new bigintmat(r, c, coeffs_BIGINT);
  for (int i=0; i<entries; i++)
  {
    (*v)[i]=ssi2ReadBigInt(d);
    if (((ssi2Info*)d)->read_failed)
    {
      delete v;
      return NULL;
    }
  }
  return v;
}

static void ssi2WriteBigintvec(const ssiInfo *d, bigintmat *v)
{
  ssi2WriteU64(d, v->cols());
  for (int i=0; i<v->length(); i++) ssi2WriteBigInt(d, (*v)[i]);
}

static bigintmat *ssi2ReadBigintvec(const ssiInfo *d)
{
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_BIGINTMAT, "ssi2")) return NULL;
  int c=0;
  if (ssi2ReadCount(d, INT_MAX, "bigintvec length", &c)) return NULL;
  bigintmat *v=new bigintmat(1, c, coeffs_BIGINT);
  for (int i=0; i<c; i++)
  {
    (*v)[i]=ssi2ReadBigInt(d);
    if (((ssi2Info*)d)->read_failed)
    {
      delete v;
      return NULL;
    }
  }
  return v;
}

static void ssi2ReadBlackbox(leftv, si_link l)
{
  /* If this is implemented later, dispatch by SSI_SCHEMA_BLACKBOX here. */
  WerrorS("ssi2: blackbox serialization is not implemented");
  ((ssi2Info*)l->data)->read_failed=TRUE;
}

static void ssi2ReadAttrib(leftv res, si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_ATTRIBUTES, "ssi2")) return;
  uint64_t flag_value=ssi2ReadU64(d);
  if (((ssi2Info*)d)->read_failed) return;
  if (flag_value>UINT_MAX)
  {
    WerrorS("ssi2: attribute flags are outside the supported range");
    ((ssi2Info*)d)->read_failed=TRUE;
    return;
  }
  BITSET fl=(BITSET)flag_value;
  int nr_of_attr=0;
  if (((ssi2Info*)d)->read_failed
  || ssi2ReadCount(d, INT_MAX, "attribute count", &nr_of_attr)) return;
  if (nr_of_attr>0)
  {
    for (int i=1; i<nr_of_attr; i++) {}
  }
  leftv tmp=ssi2Read1(l);
  if (tmp==NULL) return;
  memcpy(res, tmp, sizeof(sleftv));
  memset(tmp, 0, sizeof(sleftv));
  omFreeBin(tmp, sleftv_bin);
  res->flag=fl;
}

static void ssi2ReadRingProperties(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (ssiRequireSchemaVersion(d, SSI_SCHEMA_RING_PROPERTIES, "ssi2")) return;
  if (d->r==NULL)
  {
    WerrorS("ssi2: ring property without a ring");
    ((ssi2Info*)d)->read_failed=TRUE;
    return;
  }
  int what=0;
  if (ssi2ReadCount(d, 2, "ring property type", &what)) return;
  switch (what)
  {
    case 0:
    {
      int lb=0;
      if (ssi2ReadCount(d, sizeof(unsigned long)*CHAR_BIT-1,
                        "ring exponent size", &lb)) return;
      if (lb==0)
      {
        WerrorS("ssi2: invalid ring exponent size");
        ((ssi2Info*)d)->read_failed=TRUE;
        return;
      }
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      rComplete(d->r);
      break;
    }
    case 1:
    {
      int lb=0;
      int isLPring=0;
      if (ssi2ReadCount(d, sizeof(unsigned long)*CHAR_BIT-1,
                        "ring exponent size", &lb)
      || ssi2ReadInt(d, "localization ring flag", &isLPring)) return;
      if ((lb==0) || (isLPring<=0) || (isLPring>SHRT_MAX)
      || ((d->r->N % isLPring)!=0))
      {
        WerrorS("ssi2: invalid localization ring properties");
        ((ssi2Info*)d)->read_failed=TRUE;
        return;
      }
      unsigned long bm=~0L;
      bm=bm<<lb;
      bm=~bm;
      rUnComplete(d->r);
      d->r->bitmask=bm;
      d->r->isLPring=isLPring;
      rComplete(d->r);
      break;
    }
    case 2:
    {
      matrix C=ssi2ReadMatrix(d);
      matrix D=ssi2ReadMatrix(d);
      if ((C==NULL) || (D==NULL) || ((ssi2Info*)d)->read_failed)
      {
        if (C!=NULL) id_Delete((ideal*)&C, d->r);
        if (D!=NULL) id_Delete((ideal*)&D, d->r);
        return;
      }
      nc_CallPlural(C, D, NULL, NULL, d->r, true, true, false, d->r, false);
      break;
    }
    default:
      Werror("ssi2: unknown ring property %d", what);
      ((ssi2Info*)d)->read_failed=TRUE;
      break;
  }
}

static void ssi2WriteHeader(const ssiInfo *d)
{
  ssi2WriteTag(d, 98);
  ssi2WriteU64(d, SSI2_VERSION);
  ssi2WriteU64(d, MAX_TOK);
  ssi2WriteU64(d, si_opt_1);
  ssi2WriteU64(d, si_opt_2);
}

static leftv ssi2Read1(si_link l)
{
  if ((l==NULL) || (l->data==NULL)) return NULL;
  ssi2Info *d=(ssi2Info*)l->data;
  if (d->read_failed) return NULL;
  d->read_depth++;
  leftv result=ssi2Read1Internal(l);
  if (l->data==d) d->read_depth--;
  return result;
}

static leftv ssi2Read1Internal(si_link l)
{
  ssiInfo *d=(ssiInfo*)l->data;
  ssi2Info *dd=(ssi2Info*)d;
  if (dd->read_failed) return NULL;
  leftv res=(leftv)omAlloc0Bin(sleftv_bin);
  int t=ssi2ReadTag(d);
  if (t==-2)
  {
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  if ((t>=0) && (!dd->header_seen) && (t!=98))
  {
    WerrorS("ssi2: missing stream header");
    dd->read_failed=TRUE;
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  if ((t>=0) && dd->header_seen && (!dd->schema_table_seen)
  && (t!=SSI_SCHEMA_TOKEN))
  {
    WerrorS("ssi2: missing schema table");
    dd->read_failed=TRUE;
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  if (dd->schema_table_seen && (t==SSI_SCHEMA_TOKEN))
  {
    WerrorS("ssi2: duplicate schema table");
    dd->read_failed=TRUE;
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  if (dd->schema_table_seen && (t==98) && (dd->read_depth!=1))
  {
    WerrorS("ssi2: stream header inside a nested value");
    dd->read_failed=TRUE;
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  switch (t)
  {
    case 1:
    {
      long value=0;
      if (ssi2ReadLong(d, "integer", &value))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      res->rtyp=INT_CMD;
      res->data=(char*)value;
      break;
    }
    case 2:
      res->rtyp=STRING_CMD;
      res->data=(char*)ssi2ReadString(d);
      break;
    case 3:
      res->rtyp=NUMBER_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      res->data=(char*)ssi2ReadNumber(d);
      break;
    case 4:
      res->rtyp=BIGINT_CMD;
      res->data=(char*)ssi2ReadBigInt(d);
      break;
    case 15:
    case 5:
    {
      d->r=ssi2ReadRing(d);
      if (dd->read_failed || (d->r==NULL))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      res->data=(char*)d->r;
      if (d->r!=NULL) rIncRefCnt(d->r);
      res->rtyp=RING_CMD;
      if (t==15)
      {
        if (ssi2SetCurrRing(d->r)) d->r=currRing;
        omFreeBin(res, sleftv_bin);
        return ssi2Read1Internal(l);
      }
      break;
    }
    case 6:
      res->rtyp=POLY_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      res->data=(char*)ssi2ReadPoly(d);
      break;
    case 7:
      res->rtyp=IDEAL_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      res->data=(char*)ssi2ReadIdeal(d);
      break;
    case 8:
      res->rtyp=MATRIX_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      res->data=(char*)ssi2ReadMatrix(d);
      break;
    case 9:
      res->rtyp=VECTOR_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      res->data=(char*)ssi2ReadPoly(d);
      break;
    case 10:
    case 22:
      res->rtyp=(t==22) ? SMATRIX_CMD : MODUL_CMD;
      if (d->r==NULL) goto no_ring;
      ssi2CheckCurrRing(d->r);
      if (ssiRequireSchemaVersion(d, SSI_SCHEMA_MODULE, "ssi2"))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      {
        int rk=0;
        if (ssi2ReadInt(d, "module rank", &rk) || (rk<0))
        {
          if (!dd->read_failed)
          {
            WerrorS("ssi2: invalid module rank");
            dd->read_failed=TRUE;
          }
          omFreeBin(res, sleftv_bin);
          return NULL;
        }
        ideal M=ssi2ReadIdeal(d);
        if (M==NULL)
        {
          omFreeBin(res, sleftv_bin);
          return NULL;
        }
        M->rank=rk;
        res->data=(char*)M;
      }
      break;
    case 11:
      res->rtyp=COMMAND;
      res->data=ssi2ReadCommand(l);
      if (res->data==NULL)
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      if (res->Eval()) WerrorS("error in eval");
      break;
    case 12:
      res->rtyp=0;
      res->name=(char*)ssi2ReadString(d);
      if (res->name==NULL)
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      if (res->Eval()) WerrorS("error in name lookup");
      break;
    case 13:
      res->rtyp=PROC_CMD;
      res->data=ssi2ReadProc(d);
      break;
    case 14:
      res->rtyp=LIST_CMD;
      res->data=ssi2ReadList(l);
      break;
    case 16:
      res->rtyp=NONE;
      res->data=NULL;
      break;
    case 17:
      res->rtyp=INTVEC_CMD;
      res->data=ssi2ReadIntvec(d);
      break;
    case 18:
      res->rtyp=INTMAT_CMD;
      res->data=ssi2ReadIntmat(d);
      break;
    case 19:
      res->rtyp=BIGINTMAT_CMD;
      res->data=ssi2ReadBigintmat(d);
      break;
    case 20:
      ssi2ReadBlackbox(res, l);
      break;
    case 21:
      ssi2ReadAttrib(res, l);
      break;
    case 23:
      ssi2ReadRingProperties(l);
      omFreeBin(res, sleftv_bin);
      if (dd->read_failed) return NULL;
      return ssi2Read1Internal(l);
    case 24:
      res->rtyp=BIGINTVEC_CMD;
      res->data=ssi2ReadBigintvec(d);
      break;
    case SSI_SCHEMA_TOKEN:
      if (ssi2ReadSchemaTable(d))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      omFreeBin(res, sleftv_bin);
      return ssi2Read1Internal(l);
    case 98:
    {
      uint64_t n98_v=ssi2ReadU64(d);
      uint64_t n98_m=ssi2ReadU64(d);
      uint64_t n98_o1_value=ssi2ReadU64(d);
      uint64_t n98_o2_value=ssi2ReadU64(d);
      if (dd->read_failed)
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      // MAX_TOK records the writer's token ceiling.  Compatibility is
      // governed by the stream and per-family schema versions.
      if (n98_v!=SSI2_VERSION)
      {
        Werror("ssi2: incompatible stream version %llu/%llu (expected %d/%d)",
               (unsigned long long)n98_v, (unsigned long long)n98_m,
               SSI2_VERSION, MAX_TOK);
        dd->read_failed=TRUE;
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      if (dd->header_seen)
      {
        if (d->r!=NULL)
        {
          rKill(d->r);
          d->r=NULL;
        }
        for (int i=0; i<SI_RING_CACHE; i++)
        {
          if (d->rings[i]!=NULL) rKill(d->rings[i]);
          d->rings[i]=NULL;
        }
      }
      if ((n98_o1_value>UINT_MAX) || (n98_o2_value>UINT_MAX))
      {
        WerrorS("ssi2: option bitset is outside the supported range");
        dd->read_failed=TRUE;
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      dd->header_seen=TRUE;
      dd->schema_table_seen=FALSE;
      memset(dd->schema_versions, 0, sizeof(dd->schema_versions));
      dd->validated_schema_versions=0;
      si_opt_1=(BITSET)n98_o1_value;
      si_opt_2=(BITSET)n98_o2_value;
      omFreeBin(res, sleftv_bin);
      return ssi2Read1Internal(l);
    }
    case 99:
      omFreeBin(res, sleftv_bin);
      ssi2zClose(l);
      m2_end(-1);
      break;
    case -1:
      if ((dd->read_depth!=1) || (!dd->header_seen) || (!dd->schema_table_seen))
      {
        WerrorS("ssi2: unexpected end of input");
        dd->read_failed=TRUE;
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      if (ssi2zClose(l))
      {
        omFreeBin(res, sleftv_bin);
        return NULL;
      }
      res->rtyp=DEF_CMD;
      return res;
    default:
      Werror("ssi2: not implemented (t:%d)", t);
      dd->read_failed=TRUE;
      omFreeBin(res, sleftv_bin);
      res=NULL;
      break;
  }
  if (dd->read_failed)
  {
    if (res->data!=NULL) res->CleanUp(d->r);
    omFreeBin(res, sleftv_bin);
    return NULL;
  }
  if ((d->r!=NULL) && (currRing!=d->r) && (res!=NULL) && (res->RingDependend()))
  {
    if (ssi2SetCurrRing(d->r)) d->r=currRing;
  }
  return res;
no_ring:
  WerrorS("no ring");
  dd->read_failed=TRUE;
  omFreeBin(res, sleftv_bin);
  return NULL;
}

static BOOLEAN ssi2Write(si_link l, leftv data)
{
  if (SI_LINK_W_OPEN_P(l)==0)
    if (slOpen(l, SI_LINK_OPEN|SI_LINK_WRITE, NULL)) return TRUE;
  ssiInfo *d=(ssiInfo*)l->data;
  ssi2Info *sd=(ssi2Info*)d;
  if ((sd==NULL) || sd->write_failed) return TRUE;
  d->level++;
  while (data!=NULL)
  {
    int tt=data->Typ();
    void *dd=data->Data();
    attr *aa=data->Attribute();
    if ((aa!=NULL) && ((*aa)!=NULL))
    {
      attr a=*aa;
      int n=0;
      while (a!=NULL) { n++; a=a->next; }
      ssi2WriteTag(d, 21);
      ssi2WriteU64(d, data->flag);
      ssi2WriteU64(d, n);
    }
    else if (data->flag!=0)
    {
      ssi2WriteTag(d, 21);
      ssi2WriteU64(d, data->flag);
      ssi2WriteU64(d, 0);
    }
    if ((dd==NULL) && (data->name!=NULL) && (tt==0)) tt=DEF_CMD;
    switch (tt)
    {
      case 0:
      case NONE:
        ssi2WriteTag(d, 16);
        break;
      case STRING_CMD:
        ssi2WriteTag(d, 2);
        ssi2WriteString(d, (char*)dd);
        break;
      case INT_CMD:
        ssi2WriteTag(d, 1);
        ssi2WriteI64(d, (long)dd);
        break;
      case BIGINT_CMD:
        ssi2WriteTag(d, 4);
        ssi2WriteBigInt(d, (number)dd);
        break;
      case NUMBER_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        ssi2WriteTag(d, 3);
        ssi2WriteNumber(d, (number)dd);
        break;
      case RING_CMD:
        ssi2WriteTag(d, 5);
        ssi2WriteRing(d, (ring)dd);
        break;
      case BUCKET_CMD:
      {
        sBucket_pt b=(sBucket_pt)dd;
        if (d->r!=sBucketGetRing(b))
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, sBucketGetRing(b));
        }
        ssi2WriteTag(d, 6);
        ssi2WritePoly(d, sBucketPeek(b));
        break;
      }
      case POLY_CMD:
      case VECTOR_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        ssi2WriteTag(d, (tt==POLY_CMD) ? 6 : 9);
        ssi2WritePoly(d, (poly)dd);
        break;
      case IDEAL_CMD:
      case MODUL_CMD:
      case MATRIX_CMD:
      case SMATRIX_CMD:
        if (d->r!=currRing)
        {
          ssi2WriteTag(d, 15);
          ssi2WriteRing(d, currRing);
        }
        if (tt==IDEAL_CMD) ssi2WriteTag(d, 7);
        else if (tt==MATRIX_CMD) ssi2WriteTag(d, 8);
        else
        {
          ideal M=(ideal)dd;
          ssi2WriteTag(d, (tt==MODUL_CMD) ? 10 : 22);
          ssi2WriteI64(d, M->rank);
        }
        ssi2WriteIdeal(d, tt, (ideal)dd);
        break;
      case COMMAND:
        ssi2WriteTag(d, 11);
        ssi2WriteCommand(l, (command)dd);
        break;
      case DEF_CMD:
        ssi2WriteTag(d, 12);
        ssi2WriteString(d, data->Name());
        break;
      case PROC_CMD:
        ssi2WriteTag(d, 13);
        ssi2WriteProc(d, (procinfov)dd);
        break;
      case LIST_CMD:
        ssi2WriteTag(d, 14);
        ssi2WriteList(l, (lists)dd);
        break;
      case INTVEC_CMD:
        ssi2WriteTag(d, 17);
        ssi2WriteIntvec(d, (intvec*)dd);
        break;
      case INTMAT_CMD:
        ssi2WriteTag(d, 18);
        ssi2WriteIntmat(d, (intvec*)dd);
        break;
      case BIGINTMAT_CMD:
        ssi2WriteTag(d, 19);
        ssi2WriteBigintmat(d, (bigintmat*)dd);
        break;
      case BIGINTVEC_CMD:
        ssi2WriteTag(d, 24);
        ssi2WriteBigintvec(d, (bigintmat*)dd);
        break;
      default:
        Werror("ssi2: not implemented (t:%d, rtyp:%d)", tt, data->rtyp);
        sd->write_failed=TRUE;
        d->level--;
        return TRUE;
    }
    if (sd->write_failed)
    {
      d->level--;
      return TRUE;
    }
    if (d->level<=1) ssi2Fflush(d);
    if (sd->write_failed)
    {
      d->level--;
      return TRUE;
    }
    data=data->next;
  }
  d->level--;
  return sd->write_failed;
}

static BOOLEAN ssi2Open(si_link l, short flag, leftv)
{
  if (l==NULL) return TRUE;
  if (FE_OPT_NO_SHELL_FLAG)
  {
    WerrorS("no links allowed");
    return TRUE;
  }
  const char *link_mode=(l->mode!=NULL) ? l->mode : "";
  char base=ssi2ModeBase(link_mode);
  if (base=='?')
  {
    Werror("ssi2: invalid mode `%s'", link_mode);
    return TRUE;
  }
  char zstd_long[32];
  BOOLEAN has_long=FALSE;
  BOOLEAN has_compression=FALSE;
  ssi2Compression comp=ssi2ParseModeOptions(link_mode, zstd_long, sizeof(zstd_long),
                                            &has_long, &has_compression);
  if (comp==SSI2_COMP_INVALID) return TRUE;
  const char *mode;
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if ((comp==SSI2_COMP_NONE) && (!has_compression))
    comp=ssi2CompressionFromFilename(l->name);
  if ((has_long) && (comp!=SSI2_COMP_ZSTD))
  {
    WerrorS("ssi2: long=N is only valid with zstd compression");
    return TRUE;
  }
  if (comp!=SSI2_COMP_NONE)
    return ssi2CompressedOpenByCompression(l, flag, comp, zstd_long);
  if (flag==SI_LINK_READ) mode="r";
  else if (base=='a') mode="a";
  else mode="w";

  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssi2Info));
  char *reopen_mode=ssi2ReopenMode(link_mode, mode[0]);
  omFreeBinAddr(l->mode);
  l->mode=reopen_mode;

  ssi2Info *d=(ssi2Info*)omAlloc0(sizeof(ssi2Info));
  l->data=d;
  ssiInitSchemaVersions(d, flag!=SI_LINK_READ);
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    WerrorS("ssi2: file name required");
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }

  if (flag==SI_LINK_READ)
  {
    d->f_read=s_open_by_name(l->name);
    if ((d->f_read==NULL) || (d->f_read->fd<0))
    {
      Werror("ssi2: cannot open input file `%s'", l->name);
      if (d->f_read!=NULL) s_close(d->f_read);
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssi2Info));
      return TRUE;
    }
    SI_LINK_SET_R_OPEN_P(l);
  }
  else
  {
    char *filename=l->name;
    if (filename[0]=='>')
    {
      if (filename[1]=='>')
      {
        filename+=2;
        mode="a";
      }
      else
      {
        filename++;
        mode="w";
      }
    }
    d->f_write=myfopen(filename, mode);
    if (d->f_write==NULL)
    {
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssi2Info));
      return TRUE;
    }
    ssi2WriteHeader(d);
    ssi2WriteSchemaTable(d);
    ssi2Fflush(d);
    if (d->write_failed)
    {
      ssi2zClose(l);
      return TRUE;
    }
    SI_LINK_SET_W_OPEN_P(l);
  }
  return FALSE;
}

static BOOLEAN ssi2eOpen(si_link l, short flag, leftv)
{
  if (FE_OPT_NO_SHELL_FLAG)
  {
    WerrorS("no links allowed");
    return TRUE;
  }
#ifndef HAVE_LIBSODIUM
  WerrorS("ssi2e: authenticated encryption is unavailable; rebuild Singular with libsodium");
  return TRUE;
#else
  if (l==NULL) return TRUE;
  if (sodium_init()<0)
  {
    WerrorS("ssi2e: libsodium initialization failed");
    return TRUE;
  }

  const char *link_mode=(l->mode!=NULL) ? l->mode : "";
  char base=ssi2ModeBase(link_mode);
  char *keyfile=NULL;
  if (ssi2eParseModeOptions(link_mode, &keyfile)) return TRUE;
  if (base=='a')
  {
    WerrorS("ssi2e: append mode is not supported");
    omFree(keyfile);
    return TRUE;
  }
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    WerrorS("ssi2e: file name required");
    omFree(keyfile);
    return TRUE;
  }
  if ((flag!=SI_LINK_READ) && (l->name[0]=='>') && (l->name[1]=='>'))
  {
    WerrorS("ssi2e: append redirection is not supported");
    omFree(keyfile);
    return TRUE;
  }

  unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
  if (ssi2eReadKeyFile(keyfile, key))
  {
    omFree(keyfile);
    return TRUE;
  }
  omFree(keyfile);

  char *reopen_mode=ssi2ReopenMode(link_mode,
                                   (flag==SI_LINK_READ) ? 'r' : 'w');
  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssi2Info));
  omFreeBinAddr(l->mode);
  l->mode=reopen_mode;

  ssi2Info *d=(ssi2Info*)omAlloc0(sizeof(ssi2Info));
  l->data=d;
  ssiInitSchemaVersions(d, flag!=SI_LINK_READ);
  d->encrypted=TRUE;
  d->encryption_reading=(flag==SI_LINK_READ);
  d->compressor_name="ssi2e";
  memcpy(d->encryption_header, ssi2eFixedHeader, SSI2E_FIXED_HEADER_SIZE);

  BOOLEAN failed=FALSE;
  const char *filename=l->name;
  if ((flag!=SI_LINK_READ) && (filename[0]=='>')) filename++;
  if (flag==SI_LINK_READ)
  {
    d->encryption_input=myfopen(filename, "rb");
    if (d->encryption_input==NULL)
    {
      WerrorS("ssi2e: cannot open encrypted input");
      failed=TRUE;
    }
    if ((!failed)
    && (fread(d->encryption_header, 1, SSI2E_FIXED_HEADER_SIZE,
              d->encryption_input)!=SSI2E_FIXED_HEADER_SIZE))
    {
      WerrorS("ssi2e: truncated encrypted envelope header");
      failed=TRUE;
    }
    if ((!failed)
    && (memcmp(d->encryption_header, ssi2eFixedHeader,
               SSI2E_FIXED_HEADER_SIZE)!=0))
    {
      WerrorS("ssi2e: unsupported or invalid encrypted envelope header");
      failed=TRUE;
    }
    if ((!failed)
    && (fread(d->encryption_header+SSI2E_FIXED_HEADER_SIZE, 1,
              crypto_secretstream_xchacha20poly1305_HEADERBYTES,
              d->encryption_input)
        !=crypto_secretstream_xchacha20poly1305_HEADERBYTES))
    {
      WerrorS("ssi2e: truncated cryptographic stream header");
      failed=TRUE;
    }
    if ((!failed)
    && (crypto_secretstream_xchacha20poly1305_init_pull(
          &d->encryption_state,
          d->encryption_header+SSI2E_FIXED_HEADER_SIZE, key)!=0))
    {
      WerrorS("ssi2e: encrypted stream initialization failed");
      failed=TRUE;
    }
    sodium_memzero(key, sizeof(key));
    if (!failed)
    {
      d->encryption_read_buff=(unsigned char*)omAlloc(SSI2E_PLAINTEXT_CHUNK_SIZE);
      if (ssi2eReadFrame(d)) failed=TRUE;
      else if (d->encryption_final_seen)
      {
        WerrorS("ssi2e: encrypted envelope contains no SSI2 payload");
        failed=TRUE;
      }
    }
    if (!failed) SI_LINK_SET_R_OPEN_P(l);
  }
  else
  {
    d->f_write=myfopen(filename, "wb");
    if (d->f_write==NULL)
    {
      WerrorS("ssi2e: cannot open encrypted output");
      failed=TRUE;
    }
    if ((!failed)
    && (crypto_secretstream_xchacha20poly1305_init_push(
          &d->encryption_state,
          d->encryption_header+SSI2E_FIXED_HEADER_SIZE, key)!=0))
    {
      WerrorS("ssi2e: encrypted stream initialization failed");
      failed=TRUE;
    }
    sodium_memzero(key, sizeof(key));
    if ((!failed)
    && ((fwrite(d->encryption_header, 1, sizeof(d->encryption_header),
                d->f_write)!=sizeof(d->encryption_header))
      || (fflush(d->f_write)!=0)))
    {
      WerrorS("ssi2e: encrypted envelope header write failed");
      failed=TRUE;
    }
    if (!failed)
    {
      ssi2WriteHeader(d);
      ssi2WriteSchemaTable(d);
      ssi2Fflush(d);
      if (d->encryption_failed || d->write_failed) failed=TRUE;
    }
    if (!failed) SI_LINK_SET_W_OPEN_P(l);
  }

  if (failed)
  {
    sodium_memzero(key, sizeof(key));
    sodium_memzero(&d->encryption_state, sizeof(d->encryption_state));
    if (d->write_buff!=NULL)
    {
      sodium_memzero(d->write_buff, d->write_buff_size);
      omFreeSize(d->write_buff, d->write_buff_size);
    }
    if (d->encryption_read_buff!=NULL)
    {
      sodium_memzero(d->encryption_read_buff, SSI2E_PLAINTEXT_CHUNK_SIZE);
      omFreeSize(d->encryption_read_buff, SSI2E_PLAINTEXT_CHUNK_SIZE);
    }
    if (d->encryption_input!=NULL) fclose(d->encryption_input);
    if (d->f_write!=NULL) fclose(d->f_write);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }
  return FALSE;
#endif
}

static BOOLEAN ssi2fOpen(si_link l, short flag, leftv)
{
  if (FE_OPT_NO_SHELL_FLAG)
  {
    WerrorS("no links allowed");
    return TRUE;
  }
#ifndef HAVE_OPENSSL_FIPS
  WerrorS("ssi2f: AES-256-GCM FIPS encryption is unavailable; rebuild Singular with --with-openssl-fips=PREFIX --with-openssl-fips-provider-dir=DIR --with-openssl-fips-config=FILE");
  return TRUE;
#else
  if (l==NULL) return TRUE;

  const char *link_mode=(l->mode!=NULL) ? l->mode : "";
  char base=ssi2ModeBase(link_mode);
  char *keyfile=NULL;
  if (ssi2fParseModeOptions(link_mode, &keyfile)) return TRUE;
  if (base=='a')
  {
    WerrorS("ssi2f: append mode is not supported");
    omFree(keyfile);
    return TRUE;
  }
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    WerrorS("ssi2f: file name required");
    omFree(keyfile);
    return TRUE;
  }
  if ((flag!=SI_LINK_READ) && (l->name[0]=='>') && (l->name[1]=='>'))
  {
    WerrorS("ssi2f: append redirection is not supported");
    omFree(keyfile);
    return TRUE;
  }

  unsigned char key[SSI2F_KEY_SIZE];
  if (ssi2fReadKeyFile(keyfile, key))
  {
    omFree(keyfile);
    return TRUE;
  }
  omFree(keyfile);

  char *reopen_mode=ssi2ReopenMode(link_mode,
                                   (flag==SI_LINK_READ) ? 'r' : 'w');
  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssi2Info));
  omFreeBinAddr(l->mode);
  l->mode=reopen_mode;

  ssi2Info *d=(ssi2Info*)omAlloc0(sizeof(ssi2Info));
  l->data=d;
  ssiInitSchemaVersions(d, flag!=SI_LINK_READ);
  d->openssl_encrypted=TRUE;
  d->openssl_reading=(flag==SI_LINK_READ);
  d->compressor_name="ssi2f";
  memcpy(d->openssl_key, key, SSI2F_KEY_SIZE);
  OPENSSL_cleanse(key, sizeof(key));
  memcpy(d->openssl_header, ssi2fFixedHeader, SSI2F_FIXED_HEADER_SIZE);

  BOOLEAN failed=FALSE;
  if (ssi2fInitOpenSSL(d)) failed=TRUE;

  const char *filename=l->name;
  if ((flag!=SI_LINK_READ) && (filename[0]=='>')) filename++;
  if ((!failed) && (flag==SI_LINK_READ))
  {
    d->openssl_input=myfopen(filename, "rb");
    if (d->openssl_input==NULL)
    {
      WerrorS("ssi2f: cannot open encrypted input");
      failed=TRUE;
    }
    if ((!failed)
    && (fread(d->openssl_header, 1, SSI2F_HEADER_SIZE,
              d->openssl_input)!=SSI2F_HEADER_SIZE))
    {
      WerrorS("ssi2f: truncated encrypted envelope header");
      failed=TRUE;
    }
    if ((!failed)
    && (memcmp(d->openssl_header, ssi2fFixedHeader,
               SSI2F_FIXED_HEADER_SIZE)!=0))
    {
      WerrorS("ssi2f: unsupported or invalid encrypted envelope header");
      failed=TRUE;
    }
    if (!failed)
    {
      d->openssl_read_buff=(unsigned char*)omAlloc(SSI2F_PLAINTEXT_CHUNK_SIZE);
      if (ssi2fReadFrame(d)) failed=TRUE;
      else if (d->openssl_final_seen)
      {
        WerrorS("ssi2f: encrypted envelope contains no SSI2 payload");
        failed=TRUE;
      }
    }
    if (!failed) SI_LINK_SET_R_OPEN_P(l);
  }
  else if (!failed)
  {
    d->f_write=myfopen(filename, "wb");
    if (d->f_write==NULL)
    {
      WerrorS("ssi2f: cannot open encrypted output");
      failed=TRUE;
    }
    if ((!failed)
    && (RAND_bytes_ex(d->openssl_libctx,
                      d->openssl_header+SSI2F_FIXED_HEADER_SIZE,
                      SSI2F_NONCE_PREFIX_SIZE, 256)!=1))
    {
      WerrorS("ssi2f: cannot generate encrypted stream nonce");
      failed=TRUE;
    }
    if ((!failed)
    && ((fwrite(d->openssl_header, 1, SSI2F_HEADER_SIZE, d->f_write)
          !=SSI2F_HEADER_SIZE)
      || (fflush(d->f_write)!=0)))
    {
      WerrorS("ssi2f: encrypted envelope header write failed");
      failed=TRUE;
    }
    if (!failed)
    {
      ssi2WriteHeader(d);
      ssi2WriteSchemaTable(d);
      ssi2Fflush(d);
      if (d->openssl_failed || d->write_failed) failed=TRUE;
    }
    if (!failed) SI_LINK_SET_W_OPEN_P(l);
  }

  if (failed)
  {
    OPENSSL_cleanse(d->openssl_key, sizeof(d->openssl_key));
    if (d->write_buff!=NULL)
    {
      OPENSSL_cleanse(d->write_buff, d->write_buff_size);
      omFreeSize(d->write_buff, d->write_buff_size);
    }
    if (d->openssl_read_buff!=NULL)
    {
      OPENSSL_cleanse(d->openssl_read_buff, SSI2F_PLAINTEXT_CHUNK_SIZE);
      omFreeSize(d->openssl_read_buff, SSI2F_PLAINTEXT_CHUNK_SIZE);
    }
    if (d->openssl_input!=NULL) fclose(d->openssl_input);
    if (d->f_write!=NULL) fclose(d->f_write);
    ssi2fCleanupOpenSSL(d);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }
  return FALSE;
#endif
}

static BOOLEAN ssi2CompressedOpen(si_link l, short flag,
                                  const char *link_type,
                                  const char *program,
                                  char *const read_argv[],
                                  char *const write_argv[])
{
  if (l==NULL) return TRUE;
  if (FE_OPT_NO_SHELL_FLAG)
  {
    WerrorS("no links allowed");
    return TRUE;
  }
  const char *link_mode=(l->mode!=NULL) ? l->mode : "";
  char base=ssi2ModeBase(l->mode);
  if (base=='?')
  {
    Werror("%s: invalid mode `%s'", link_type, l->mode);
    return TRUE;
  }
  const char *mode;
  if (flag & SI_LINK_OPEN)
  {
    if (base=='r') flag=SI_LINK_READ;
    else flag=SI_LINK_WRITE;
  }
  if (flag==SI_LINK_READ) mode="r";
  else if (base=='a') mode="a";
  else mode="w";

  SI_LINK_SET_OPEN_P(l, flag);
  if (l->data!=NULL) omFreeSize(l->data, sizeof(ssi2Info));
  char *reopen_mode=ssi2ReopenMode(link_mode, mode[0]);
  omFreeBinAddr(l->mode);
  l->mode=reopen_mode;

  ssi2Info *d=(ssi2Info*)omAlloc0(sizeof(ssi2Info));
  l->data=d;
  ssiInitSchemaVersions(d, flag!=SI_LINK_READ);
  d->compressor_name=link_type;
  if ((l->name==NULL) || (l->name[0]=='\0'))
  {
    Werror("%s: file name required", link_type);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }

  char *filename=l->name;
  if (flag!=SI_LINK_READ && filename[0]=='>')
  {
    if (filename[1]=='>')
    {
      filename+=2;
      mode="a";
    }
    else
    {
      filename++;
      mode="w";
    }
  }

  if (flag==SI_LINK_READ)
  {
    int input_fd=si_open(filename, O_RDONLY);
    if (input_fd<0)
    {
      Werror("%s: cannot open input file `%s'", link_type, filename);
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssi2Info));
      return TRUE;
    }
    si_close(input_fd);
  }

  int pc[2];
  if (pipe(pc)!=0)
  {
    Werror("%s: pipe failed with %d", link_type, errno);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }

  pid_t pid=fork();
  if (pid==-1 && errno==EAGAIN)
  {
    raise_rlimit_nproc();
    pid=fork();
  }
  if (pid==-1)
  {
    Werror("%s: could not fork %s", link_type, program);
    si_close(pc[0]);
    si_close(pc[1]);
    l->data=NULL;
    l->flags=0;
    omFreeSize(d, sizeof(ssi2Info));
    return TRUE;
  }

  if (pid==0)
  {
    if (flag==SI_LINK_READ)
    {
      int fd=si_open(filename, O_RDONLY);
      if (fd<0) _exit(126);
      si_close(pc[0]);
      si_dup2(fd, STDIN_FILENO);
      si_dup2(pc[1], STDOUT_FILENO);
      si_close(fd);
      si_close(pc[1]);
      execvp(program, read_argv);
      _exit(127);
    }
    else
    {
      int open_flags=O_WRONLY | O_CREAT;
      open_flags |= (strcmp(mode, "a")==0) ? O_APPEND : O_TRUNC;
      int fd=si_open(filename, open_flags, 0666);
      if (fd<0) _exit(126);
      si_close(pc[1]);
      si_dup2(pc[0], STDIN_FILENO);
      si_dup2(fd, STDOUT_FILENO);
      si_close(pc[0]);
      si_close(fd);
      execvp(program, write_argv);
      _exit(127);
    }
  }

  d->pid=pid;
  if (flag==SI_LINK_READ)
  {
    si_close(pc[1]);
    d->fd_read=pc[0];
    d->f_read=s_open(pc[0]);
    SI_LINK_SET_R_OPEN_P(l);
  }
  else
  {
    si_close(pc[0]);
    d->fd_write=pc[1];
    d->f_write=fdopen(pc[1], "w");
    if (d->f_write==NULL)
    {
      si_close(pc[1]);
      kill(pid, SIGTERM);
      si_waitpid(pid, NULL, 0);
      l->data=NULL;
      l->flags=0;
      omFreeSize(d, sizeof(ssi2Info));
      return TRUE;
    }
    ssi2WriteHeader(d);
    ssi2WriteSchemaTable(d);
    ssi2Fflush(d);
    if (d->write_failed)
    {
      ssi2zClose(l);
      return TRUE;
    }
    SI_LINK_SET_W_OPEN_P(l);
  }
  return FALSE;
}

static BOOLEAN ssi2CompressedOpenByCompression(si_link l, short flag,
                                               ssi2Compression comp,
                                               const char *zstd_long)
{
  switch (comp)
  {
    case SSI2_COMP_GZIP:
    {
      char *const read_argv[]={(char*)"gzip", (char*)"-cd", NULL};
      char *const write_argv[]={(char*)"gzip", (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:gzip", "gzip", read_argv, write_argv);
    }
    case SSI2_COMP_ZSTD:
    {
      char *const read_argv[]={(char*)"zstd", (char*)"-q", (char*)"-d", (char*)"-c", NULL};
      char *const write_argv[]={(char*)"zstd", (char*)"-q", (char*)"-3",
        (char*)((zstd_long!=NULL) ? zstd_long : "--long=23"), (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:zstd", "zstd", read_argv, write_argv);
    }
    case SSI2_COMP_LZ4:
    {
      char *const read_argv[]={(char*)"lz4", (char*)"-q", (char*)"-d", (char*)"-c", NULL};
      char *const write_argv[]={(char*)"lz4", (char*)"-q", (char*)"-1", (char*)"-c", NULL};
      return ssi2CompressedOpen(l, flag, "ssi2:lz4", "lz4", read_argv, write_argv);
    }
    default:
      WerrorS("ssi2: invalid compression option");
      return TRUE;
  }
}

static BOOLEAN ssi2zOpen(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_GZIP, NULL);
}

static BOOLEAN ssi2cOpen(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_ZSTD, "--long=23");
}

static BOOLEAN ssi2zstdOpen(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_ZSTD, "--long=23");
}

static BOOLEAN ssi2lz4Open(si_link l, short flag, leftv u)
{
  return ssi2CompressedOpenByCompression(l, flag, SSI2_COMP_LZ4, NULL);
}

static BOOLEAN ssi2zClose(si_link l)
{
  BOOLEAN res=FALSE;
  if (l!=NULL)
  {
    SI_LINK_SET_CLOSE_P(l);
    ssi2Info *d=(ssi2Info*)l->data;
    if (d!=NULL)
    {
      if (d->read_failed || d->write_failed) res=TRUE;
      if (d->r!=NULL) rKill(d->r);
      for (int i=0; i<SI_RING_CACHE; i++)
      {
        if (d->rings[i]!=NULL) rKill(d->rings[i]);
        d->rings[i]=NULL;
      }
      BOOLEAN was_read=(d->f_read!=NULL);
#ifdef HAVE_LIBSODIUM
      if (d->encrypted && d->encryption_reading)
      {
        was_read=TRUE;
        if (ssi2eDrain(d)) res=TRUE;
      }
#endif
#ifdef HAVE_OPENSSL_FIPS
      if (d->openssl_encrypted && d->openssl_reading)
      {
        was_read=TRUE;
        if (ssi2fDrain(d)) res=TRUE;
      }
#endif
      if (d->f_read!=NULL) { s_close(d->f_read); d->f_read=NULL; }
      ssi2FreeWriteBuffer(d);
      if (d->write_failed) res=TRUE;
#ifdef HAVE_LIBSODIUM
      if (d->encrypted && (!d->encryption_reading)
      && (!d->encryption_final_written) && (!d->encryption_failed))
      {
        if (ssi2eWriteFrame(d, NULL, 0,
              crypto_secretstream_xchacha20poly1305_TAG_FINAL))
          res=TRUE;
        else
          d->encryption_final_written=TRUE;
      }
#endif
#ifdef HAVE_OPENSSL_FIPS
      if (d->openssl_encrypted && (!d->openssl_reading)
      && (!d->openssl_final_written) && (!d->openssl_failed))
      {
        if (ssi2fWriteFrame(d, NULL, 0, TRUE))
          res=TRUE;
        else
          d->openssl_final_written=TRUE;
      }
#endif
      if (d->f_write!=NULL) { if (fclose(d->f_write)!=0) res=TRUE; d->f_write=NULL; }
#ifdef HAVE_LIBSODIUM
      if (d->encrypted)
      {
        if (d->encryption_input!=NULL)
        {
          if (fclose(d->encryption_input)!=0) res=TRUE;
          d->encryption_input=NULL;
        }
        if (d->encryption_read_buff!=NULL)
        {
          sodium_memzero(d->encryption_read_buff, SSI2E_PLAINTEXT_CHUNK_SIZE);
          omFreeSize(d->encryption_read_buff, SSI2E_PLAINTEXT_CHUNK_SIZE);
          d->encryption_read_buff=NULL;
        }
        sodium_memzero(&d->encryption_state, sizeof(d->encryption_state));
        if (d->encryption_failed) res=TRUE;
      }
#endif
#ifdef HAVE_OPENSSL_FIPS
      if (d->openssl_encrypted)
      {
        if (d->openssl_input!=NULL)
        {
          if (fclose(d->openssl_input)!=0) res=TRUE;
          d->openssl_input=NULL;
        }
        if (d->openssl_read_buff!=NULL)
        {
          OPENSSL_cleanse(d->openssl_read_buff, SSI2F_PLAINTEXT_CHUNK_SIZE);
          omFreeSize(d->openssl_read_buff, SSI2F_PLAINTEXT_CHUNK_SIZE);
          d->openssl_read_buff=NULL;
        }
        OPENSSL_cleanse(d->openssl_key, sizeof(d->openssl_key));
        ssi2fCleanupOpenSSL(d);
        if (d->openssl_failed) res=TRUE;
      }
#endif
      if (d->pid>1)
      {
        const char *compressor_name=(d->compressor_name!=NULL) ? d->compressor_name : "ssi2z";
        int status=0;
        if (si_waitpid(d->pid, &status, 0)!=d->pid)
          res=TRUE;
        else if ((status!=0)
        && !(was_read && WIFSIGNALED(status) && (WTERMSIG(status)==SIGPIPE)))
        {
          Werror("%s: compressor exited with status %d", compressor_name, status);
          res=TRUE;
        }
      }
      l->data=NULL;
      omFreeSize(d, sizeof(ssi2Info));
    }
  }
  return res;
}

static const char* slStatusSsi2(si_link l, const char* request)
{
  ssiInfo *d=(ssiInfo*)l->data;
  if (strcmp(request, "encryption")==0)
  {
    if (l->m==NULL)
      return "none";
    if (strcmp(l->m->type, "ssi2e")==0)
    {
#ifdef HAVE_LIBSODIUM
      return "xchacha20poly1305";
#else
      return "unavailable";
#endif
    }
    if (strcmp(l->m->type, "ssi2f")==0)
    {
#ifdef HAVE_OPENSSL_FIPS
      return "aes-256-gcm-fips";
#else
      return "unavailable";
#endif
    }
    return "none";
  }
  if (strcmp(request, "read")==0)
  {
    ssi2Info *state=(ssi2Info*)d;
    if ((state!=NULL) && state->read_failed) return "not ready";
#ifdef HAVE_LIBSODIUM
    ssi2Info *dd=(ssi2Info*)d;
    if (SI_LINK_R_OPEN_P(l) && (dd!=NULL) && dd->encrypted
    && (!dd->encryption_failed) && (!dd->encryption_final_seen))
      return "ready";
#endif
#ifdef HAVE_OPENSSL_FIPS
    ssi2Info *dd_open_ssl=(ssi2Info*)d;
    if (SI_LINK_R_OPEN_P(l) && (dd_open_ssl!=NULL) && dd_open_ssl->openssl_encrypted
    && (!dd_open_ssl->openssl_failed) && (!dd_open_ssl->openssl_final_seen))
      return "ready";
#endif
    if (SI_LINK_R_OPEN_P(l) && (d!=NULL) && (d->f_read!=NULL) && (!s_iseof(d->f_read)))
      return "ready";
    return "not ready";
  }
  if (strcmp(request, "write")==0)
  {
    ssi2Info *state=(ssi2Info*)d;
    if (SI_LINK_W_OPEN_P(l) && ((state==NULL) || (!state->write_failed)))
      return "ready";
    return "not ready";
  }
  return "unknown status request";
}

si_link_extension slInitSsi2Extension(si_link_extension s)
{
  s->Open=ssi2Open;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2";
  return s;
}

si_link_extension slInitSsi2cExtension(si_link_extension s)
{
  s->Open=ssi2cOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2c";
  return s;
}

si_link_extension slInitSsi2zExtension(si_link_extension s)
{
  s->Open=ssi2zOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2z";
  return s;
}

si_link_extension slInitSsi2zstdExtension(si_link_extension s)
{
  s->Open=ssi2zstdOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2zstd";
  return s;
}

si_link_extension slInitSsi2lz4Extension(si_link_extension s)
{
  s->Open=ssi2lz4Open;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2lz4";
  return s;
}

si_link_extension slInitSsi2eExtension(si_link_extension s)
{
  s->Open=ssi2eOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2e";
  return s;
}

si_link_extension slInitSsi2fExtension(si_link_extension s)
{
  s->Open=ssi2fOpen;
  s->Close=ssi2zClose;
  s->Kill=ssi2zClose;
  s->Read=ssi2Read1;
  s->Read2=NULL;
  s->Write=ssi2Write;
  s->Dump=NULL;
  s->GetDump=NULL;
  s->Status=slStatusSsi2;
  s->SetRing=NULL;
  s->type="ssi2f";
  return s;
}
