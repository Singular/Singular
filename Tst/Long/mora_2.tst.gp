Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 61.20     47.70    47.70   312360     0.15     0.15  p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
 27.65     69.25    21.55     9135     2.36     8.23  redEcart(sLObject *, skStrategy *)
  2.05     70.85     1.60                             __mcount_internal
  1.19     71.78     0.93   107354     0.01     0.01  p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
  0.81     72.41     0.63   424837     0.00     0.00  p_GetShortExpVector(spolyrec *, sip_sring *)
  0.77     73.01     0.60   415695     0.00     0.12  ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *)
  0.73     73.58     0.57   205503     0.00     0.00  pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
  0.72     74.14     0.56   415724     0.00     0.00  p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket
  0.64     74.64     0.50                             mcount
  0.51     75.04     0.40   415710     0.00     0.12  kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *)
  0.49     75.42     0.38      820     0.46     0.69  pNorm(spolyrec *)
  0.42     75.75     0.33                             write
  0.36     76.03     0.28   415717     0.00     0.00  kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int)
  0.32     76.28     0.25   415695     0.00     0.13  doRed(sLObject *, sTObject *, short, skStrategy *)
  0.30     76.51     0.23   234081     0.00     0.00  posInL17(sLObject *, int, sLObject *, skStrategy *)
  0.24     76.70     0.19   747137     0.00     0.00  nDummy1(snumber **)
  0.23     76.88     0.18      826     0.22     0.24  p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
  0.19     77.03     0.15     1632     0.09     0.11  p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
  0.18     77.17     0.14   415907     0.00     0.00  pDeg(spolyrec *, sip_sring *)
  0.13     77.27     0.10   747000     0.00     0.00  npIsOne(snumber *)
  0.13     77.37     0.10     2301     0.04     0.04  omAllocBinPage
  0.13     77.47     0.10      885     0.11     0.12  p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
  0.12     77.56     0.09    32070     0.00     0.01  omAllocBinFromFullPage
  0.05     77.60     0.04   330407     0.00     0.00  npDiv(snumber *, snumber *)
  0.05     77.64     0.04    34297     0.00     0.00  omFreeToPageFault
  0.05     77.68     0.04                             memmove
  0.03     77.70     0.02   331218     0.00     0.00  nDummy2(snumber *&)
  0.03     77.72     0.02        1    20.00 75313.45  mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *)
  0.03     77.74     0.02                             _IO_file_xsputn
  0.03     77.76     0.02                             fork
  0.03     77.78     0.02                             fwrite
  0.01     77.79     0.01   206700     0.00     0.00  npNeg(snumber *)
  0.01     77.80     0.01     9263     0.00     0.00  enterL(sLObject **, int *, int *, sLObject, int)
  0.01     77.81     0.01     9141     0.00     0.00  PrintS
  0.01     77.82     0.01     9135     0.00     0.00  message(int, int *, int *, skStrategy *)
  0.01     77.83     0.01     9098     0.00     0.00  kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *)
  0.01     77.84     0.01     1822     0.01     0.01  yylex(MYYSTYPE *)
  0.01     77.85     0.01      842     0.01     0.01  kBucketCreate(sip_sring *)
  0.01     77.86     0.01      827     0.01     0.25  enterT(sLObject, skStrategy *, int)
  0.01     77.87     0.01       69     0.14     0.14  omAllocFromSystem
  0.01     77.88     0.01        1    10.00    10.29  npInitChar(int, sip_sring *)
  0.01     77.89     0.01        1    10.00    10.29  npSetChar(int, sip_sring *)
  0.01     77.90     0.01                             _IO_do_write
  0.01     77.91     0.01                             _IO_file_sync
  0.01     77.92     0.01                             _IO_file_write
  0.01     77.93     0.01                             _IO_str_init_static
  0.01     77.94     0.01                             mmap
  0.00     77.94     0.00     2262     0.00     0.00  omFreeBinPages
  0.00     77.94     0.00     1654     0.00     0.03  kBucketCanonicalize(kBucket *)
  0.00     77.94     0.00     1176     0.00     0.00  sleftv::Typ(void)
  0.00     77.94     0.00      988     0.00     0.00  sleftv::CleanUp(void)
  0.00     77.94     0.00      875     0.00     0.00  npInit(int)
  0.00     77.94     0.00      842     0.00     0.00  kBucketDestroy(kBucket **)
  0.00     77.94     0.00      842     0.00     0.00  kBucketInit(kBucket *, spolyrec *, int)
  0.00     77.94     0.00      827     0.00     0.03  kBucketClear(kBucket *, spolyrec **, int *)
  0.00     77.94     0.00      827     0.00     0.00  posInT17(sTObject *, int, sLObject const &)
  0.00     77.94     0.00      790     0.00     0.00  feReadLine(char *, int)
  0.00     77.94     0.00      790     0.00     0.00  yy_get_next_buffer(void)
  0.00     77.94     0.00      774     0.00     0.00  yy_get_previous_state(void)
  0.00     77.94     0.00      575     0.00     0.00  sleftv::Data(void)
  0.00     77.94     0.00      560     0.00     0.00  iiCheckRing(int)
  0.00     77.94     0.00      512     0.00     0.00  IsCmd(char *, int &)
  0.00     77.94     0.00      443     0.00     0.00  p_Setm_TotalDegree(spolyrec *, sip_sring *)
  0.00     77.94     0.00      403     0.00     0.00  current_pos(int)
  0.00     77.94     0.00      379     0.00     0.00  fePrintEcho(char *, char *)
  0.00     77.94     0.00      347     0.00     0.00  copy_string(lp_modes)
  0.00     77.94     0.00      346     0.00     0.00  sleftv::listLength(void)
  0.00     77.94     0.00      341     0.00     0.00  malloc_usable_size
  0.00     77.94     0.00      338     0.00     0.00  omSizeOfLargeAddr
  0.00     77.94     0.00      330     0.00     0.00  ggetid(char const *, short)
  0.00     77.94     0.00      330     0.00     0.02  syMake(sleftv *, char *, idrec *)
  0.00     77.94     0.00      239     0.00     0.00  iiTabIndex(sValCmdTab const *, int, int)
  0.00     77.94     0.00      172     0.00     0.01  iiExprArith2(sleftv *, sleftv *, int, sleftv *, short)
  0.00     77.94     0.00      167     0.00     0.00  __builtin_vec_new
  0.00     77.94     0.00      162     0.00     0.00  omGetUsedBytesOfBin
  0.00     77.94     0.00      156     0.00     0.00  Array<CanonicalForm>::operator[](int) const
  0.00     77.94     0.00      149     0.00     0.00  fREe
  0.00     77.94     0.00      141     0.00     0.00  enterOnePair(int, spolyrec *, int, int, skStrategy *, int)
  0.00     77.94     0.00      141     0.00     0.00  pHasNotCF(spolyrec *, spolyrec *)
  0.00     77.94     0.00      141     0.00     0.00  pLcm(spolyrec *, spolyrec *, spolyrec *)
  0.00     77.94     0.00      139     0.00     0.00  omReallocSizeFromSystem
  0.00     77.94     0.00      139     0.00     0.00  rEALLOc
  0.00     77.94     0.00      133     0.00     0.00  sleftv::CopyD(int)
  0.00     77.94     0.00      126     0.00     0.00  omDoRealloc
  0.00     77.94     0.00      125     0.00     0.00  rIsRingVar(char *)
  0.00     77.94     0.00      122     0.00     0.00  enterid(char *, int, int, idrec **, short)
  0.00     77.94     0.00      122     0.00     0.00  idrec::set(char *, int, int, short)
  0.00     77.94     0.00      111     0.00     0.00  jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00      107     0.00     0.00  initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int)
  0.00     77.94     0.00      107     0.00     0.00  ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *)
  0.00     77.94     0.00      106     0.00     0.01  iiAssign(sleftv *, sleftv *)
  0.00     77.94     0.00      106     0.00     0.00  pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
  0.00     77.94     0.00       97     0.00     0.00  yy_load_buffer_state(void)
  0.00     77.94     0.00       94     0.00     0.00  sleftv::LData(void)
  0.00     77.94     0.00       93     0.00     0.01  jiAssign_1(sleftv *, sleftv *)
  0.00     77.94     0.00       90     0.00     0.00  jjPLUS_S(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       88     0.00     0.00  iiExprArithM(sleftv *, sleftv *, int)
  0.00     77.94     0.00       86     0.00     0.00  omRealloc0Large
  0.00     77.94     0.00       85     0.00     0.00  Print
  0.00     77.94     0.00       82     0.00     0.00  Array<CanonicalForm>::~Array(void)
  0.00     77.94     0.00       82     0.00     0.00  Array<CanonicalForm>::operator=(Array<CanonicalForm> const &)
  0.00     77.94     0.00       82     0.00     0.00  __builtin_vec_delete
  0.00     77.94     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(void)
  0.00     77.94     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(int)
  0.00     77.94     0.00       82     0.00     0.00  mALLOc
  0.00     77.94     0.00       79     0.00     0.00  iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short)
  0.00     77.94     0.00       79     0.00     0.00  iiTestConvert(int, int)
  0.00     77.94     0.00       78     0.00     0.00  deleteInL(sLObject *, int *, int, skStrategy *)
  0.00     77.94     0.00       74     0.00     0.00  iiConvert(int, int, int, sleftv *, sleftv *)
  0.00     77.94     0.00       69     0.00     0.00  killhdl(idrec *, idrec **)
  0.00     77.94     0.00       67     0.00  1124.08  iiExprArith1(sleftv *, sleftv *, int)
  0.00     77.94     0.00       66     0.00     0.07  yy_flex_alloc(unsigned int)
  0.00     77.94     0.00       65     0.00     0.00  jiA_STRING(sleftv *, sleftv *, _ssubexpr *)
  0.00     77.94     0.00       65     0.00     0.00  yy_flush_buffer(yy_buffer_state *)
  0.00     77.94     0.00       65     0.00     0.00  yy_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     77.94     0.00       64     0.00     0.00  __builtin_new
  0.00     77.94     0.00       64     0.00     0.00  yy_flex_free(void *)
  0.00     77.94     0.00       63     0.00     0.00  __builtin_delete
  0.00     77.94     0.00       61     0.00     0.00  exitVoice(void)
  0.00     77.94     0.00       60     0.00     0.08  newBuffer(char *, feBufferTypes, procinfo *, int)
  0.00     77.94     0.00       59     0.00     0.00  omFreeSizeToSystem
  0.00     77.94     0.00       58     0.00     0.00  ndCopy(snumber *)
  0.00     77.94     0.00       49     0.00     0.00  npRead(char *, snumber **)
  0.00     77.94     0.00       49     0.00     0.12  pmInit(char *, short &)
  0.00     77.94     0.00       45     0.00     0.01  iiParameter(sleftv *)
  0.00     77.94     0.00       44     0.00     0.00  sleftv::String(void *, short, int)
  0.00     77.94     0.00       44     0.00     0.00  rIsPolyVar(int, sip_sring *)
  0.00     77.94     0.00       42     0.00     0.00  _omGetSpecBin
  0.00     77.94     0.00       41     0.00     0.00  iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short)
  0.00     77.94     0.00       36     0.00     0.01  malloc
  0.00     77.94     0.00       34     0.00     0.00  free
  0.00     77.94     0.00       33     0.00     0.00  feCleanUpFile(char *)
  0.00     77.94     0.00       33     0.00     0.00  jjSTRING_PL(sleftv *, sleftv *)
  0.00     77.94     0.00       33     0.00     0.00  namerec::push(sip_package *, char *, int, short)
  0.00     77.94     0.00       33     0.00     1.23  yyparse(void)
  0.00     77.94     0.00       32     0.00     0.00  sleftv::Copy(sleftv *)
  0.00     77.94     0.00       32     0.00     0.00  exitBuffer(feBufferTypes)
  0.00     77.94     0.00       32     0.00     0.00  iiCheckNest(void)
  0.00     77.94     0.00       32     0.00     0.00  iiConvName(char *)
  0.00     77.94     0.00       32     0.00     0.00  iiMake_proc(idrec *, sleftv *)
  0.00     77.94     0.00       32     0.00     0.08  iiPStart(idrec *, sleftv *)
  0.00     77.94     0.00       32     0.00     0.00  ipMoveId(idrec *)
  0.00     77.94     0.00       32     0.00     0.00  killlocals(int)
  0.00     77.94     0.00       32     0.00     0.14  myynewbuffer(void)
  0.00     77.94     0.00       32     0.00     0.00  myyoldbuffer(void *)
  0.00     77.94     0.00       32     0.00     0.00  namerec::pop(short)
  0.00     77.94     0.00       32     0.00     0.00  yyrestart(_IO_FILE *)
  0.00     77.94     0.00       29     0.00     0.00  ksCheckCoeff(snumber **, snumber **)
  0.00     77.94     0.00       29     0.00     0.14  ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **)
  0.00     77.94     0.00       29     0.00     0.00  ndGcd(snumber *, snumber *)
  0.00     77.94     0.00       26     0.00     0.00  sleftv::Print(sleftv *, int)
  0.00     77.94     0.00       26     0.00     0.00  myfread(void *, unsigned int, unsigned int, _IO_FILE *)
  0.00     77.94     0.00       26     0.00     0.00  npIsZero(snumber *)
  0.00     77.94     0.00       25     0.00     0.00  jiA_INT(sleftv *, sleftv *, _ssubexpr *)
  0.00     77.94     0.00       25     0.00     0.00  jjSYSTEM(sleftv *, sleftv *)
  0.00     77.94     0.00       22     0.00     0.00  cancelunit(sLObject *)
  0.00     77.94     0.00       21     0.00     0.00  npMult(snumber *, snumber *)
  0.00     77.94     0.00       20     0.00     0.00  feGetResourceConfig(char)
  0.00     77.94     0.00       20     0.00     0.00  feResource(feResourceConfig_s *, int)
  0.00     77.94     0.00       18     0.00     0.00  HEckeTest(spolyrec *, skStrategy *)
  0.00     77.94     0.00       18     0.00     0.01  chainCrit(spolyrec *, int, skStrategy *)
  0.00     77.94     0.00       18     0.00     0.00  enterSBba(sLObject, int, skStrategy *, int)
  0.00     77.94     0.00       18     0.00     0.00  enterSMora(sLObject, int, skStrategy *, int)
  0.00     77.94     0.00       18     0.00     0.03  enterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     77.94     0.00       18     0.00     0.00  iiS2Link(void *)
  0.00     77.94     0.00       18     0.00     0.02  initenterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     77.94     0.00       18     0.00     0.00  jjCOUNT_L(sleftv *, sleftv *)
  0.00     77.94     0.00       18     0.00     0.00  jjGT_I(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       18     0.00     0.00  jjKLAMMER_PL(sleftv *, sleftv *)
  0.00     77.94     0.00       18     0.00     0.00  jjPROC(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       18     0.00     0.00  p_IsPurePower(spolyrec *, sip_sring *)
  0.00     77.94     0.00       18     0.00     0.00  redtail(spolyrec *, int, skStrategy *)
  0.00     77.94     0.00       18     0.00     0.00  slCleanUp(sip_link *)
  0.00     77.94     0.00       18     0.00     0.00  slCloseAscii(sip_link *)
  0.00     77.94     0.00       18     0.00     0.00  slInit(sip_link *, char *)
  0.00     77.94     0.00       18     0.00     0.00  slKill(sip_link *)
  0.00     77.94     0.00       18     0.00     0.00  slOpenAscii(sip_link *, short)
  0.00     77.94     0.00       17     0.00     0.00  _omVallocFromSystem
  0.00     77.94     0.00       17     0.00     0.15  omAllocNewBinPagesRegion
  0.00     77.94     0.00       17     0.00     0.01  omBinPageIndexFault
  0.00     77.94     0.00       17     0.00     0.01  omRegisterBinPages
  0.00     77.94     0.00       17     0.00     0.00  posInS(spolyrec **, int, spolyrec *)
  0.00     77.94     0.00       16     0.00     0.00  jjDEFINED(sleftv *, sleftv *)
  0.00     77.94     0.00       16     0.00     0.00  omFreeBinPagesRegion
  0.00     77.94     0.00       16     0.00     0.00  omUnregisterBinPages
  0.00     77.94     0.00       16     0.00     0.00  omVfreeToSystem
  0.00     77.94     0.00       16     0.00     0.00  p_ISet(int, sip_sring *)
  0.00     77.94     0.00       14     0.00     0.00  FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &)
  0.00     77.94     0.00       14     0.00     0.00  _omFindInSortedList
  0.00     77.94     0.00       14     0.00     0.00  jjMINUS_I(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       14     0.00     0.00  jjOP_REST(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       14     0.00     0.09  jjPOWER_P(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00       14     0.00     0.00  jjPROC1(sleftv *, sleftv *)
  0.00     77.94     0.00       14     0.00     0.09  pPower(spolyrec *, int)
  0.00     77.94     0.00       13     0.00     0.00  feFopen(char *, char *, char *, int, int)
  0.00     77.94     0.00       12     0.00     0.00  jjA_L_LIST(sleftv *, sleftv *)
  0.00     77.94     0.00       11     0.00     0.00  iiWRITE(sleftv *, sleftv *)
  0.00     77.94     0.00       11     0.00     0.00  malloc_extend_top
  0.00     77.94     0.00       11     0.00     0.00  p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
  0.00     77.94     0.00       11     0.00     0.00  slWriteAscii(sip_link *, sleftv *)
  0.00     77.94     0.00       11     0.00     0.00  slWrite(sip_link *, sleftv *)
  0.00     77.94     0.00        9     0.00     0.00  lRingDependend(slists *)
  0.00     77.94     0.00        9     0.00     0.00  libread(_IO_FILE *, char *, int)
  0.00     77.94     0.00        9     0.00     0.06  slInternalCopy(sleftv *, int, void *, _ssubexpr *)
  0.00     77.94     0.00        9     0.00     0.00  yy_get_next_buffer(void)
  0.00     77.94     0.00        8     0.00     0.00  feResource(char, int)
  0.00     77.94     0.00        7     0.00     0.00  sleftv::Eval(void)
  0.00     77.94     0.00        7     0.00     0.00  iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.00  jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.02  jjPLUS_P(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.00  jjREAD(sleftv *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.13  jjTIMES_P(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.00  slReadAscii2(sip_link *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.00  slReadAscii(sip_link *)
  0.00     77.94     0.00        7     0.00     0.00  slRead(sip_link *, sleftv *)
  0.00     77.94     0.00        7     0.00     0.00  yy_get_previous_state(void)
  0.00     77.94     0.00        6     0.00     0.00  _omInsertInSortedList
  0.00     77.94     0.00        6     0.00     0.00  feCleanResourceValue(feResourceType, char *)
  0.00     77.94     0.00        6     0.00     0.00  feInitResource(feResourceConfig_s *, int)
  0.00     77.94     0.00        6     0.00     0.00  feVerifyResourceValue(feResourceType, char *)
  0.00     77.94     0.00        6     0.00     0.00  idInit(int, int)
  0.00     77.94     0.00        6     0.00     0.00  omSizeOfAddr
  0.00     77.94     0.00        5     0.00     0.00  sLObject::GetpLength(void)
  0.00     77.94     0.00        5     0.00     0.01  iiGetLibProcBuffer(procinfo *, int)
  0.00     77.94     0.00        5     0.00     0.00  iiProcArgs(char *, short)
  0.00     77.94     0.00        5     0.00     0.00  npEqual(snumber *, snumber *)
  0.00     77.94     0.00        5     0.00     0.00  pLength(spolyrec *)
  0.00     77.94     0.00        4     0.00     0.00  deleteHC(sLObject *, skStrategy *, short)
  0.00     77.94     0.00        4     0.00     0.00  eati(char *, int *)
  0.00     77.94     0.00        4     0.00     0.00  feSprintf(char *, char const *, int)
  0.00     77.94     0.00        4     0.00     0.00  idrec::get(char const *, int)
  0.00     77.94     0.00        4     0.00     0.30  id_Delete(sip_sideal **, sip_sring *)
  0.00     77.94     0.00        4     0.00     0.00  initEcartNormal(sLObject *)
  0.00     77.94     0.00        4     0.00     0.00  jjINDEX_I(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        4     0.00     0.00  jjMakeSub(sleftv *)
  0.00     77.94     0.00        4     0.00     0.00  make_version(char *, int)
  0.00     77.94     0.00        4     0.00     0.00  mystrcpy(char *, char *)
  0.00     77.94     0.00        4     0.00     0.00  pDiff(spolyrec *, int)
  0.00     77.94     0.00        4     0.00     0.00  pLDeg0c(spolyrec *, int *, sip_sring *)
  0.00     77.94     0.00        4     0.00     0.00  p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
  0.00     77.94     0.00        4     0.00     0.07  yy_flex_alloc(unsigned int)
  0.00     77.94     0.00        4     0.00     0.00  yy_flex_free(void *)
  0.00     77.94     0.00        4     0.00     0.00  yylp_flush_buffer(yy_buffer_state *)
  0.00     77.94     0.00        4     0.00     0.00  yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     77.94     0.00        4     0.00     0.00  yylp_load_buffer_state(void)
  0.00     77.94     0.00        3     0.00     0.00  calloc
  0.00     77.94     0.00        3     0.00     0.00  jjMEMORY(sleftv *, sleftv *)
  0.00     77.94     0.00        3     0.00     0.00  malloc_update_mallinfo
  0.00     77.94     0.00        3     0.00     0.00  omGetUsedBinBytes
  0.00     77.94     0.00        3     0.00     0.00  omUpdateInfo
  0.00     77.94     0.00        3     0.00     0.00  rDBTest(sip_sring *, char *, int)
  0.00     77.94     0.00        2     0.00     0.00  feCleanUpPath(char *)
  0.00     77.94     0.00        2     0.00     0.00  feGetOptIndex(char const *)
  0.00     77.94     0.00        2     0.00     0.00  fe_reset_fe
  0.00     77.94     0.00        2     0.00     0.00  fe_reset_input_mode(void)
  0.00     77.94     0.00        2     0.00     0.00  freeSize
  0.00     77.94     0.00        2     0.00     0.00  getTimer(void)
  0.00     77.94     0.00        2     0.00     0.15  iiLibCmd(char *, short)
  0.00     77.94     0.00        2     0.00     0.15  iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short)
  0.00     77.94     0.00        2     0.00     0.14  initL(void)
  0.00     77.94     0.00        2     0.00     0.30  jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *)
  0.00     77.94     0.00        2     0.00     0.00  mmInit(void)
  0.00     77.94     0.00        2     0.00     0.00  newFile(char *, _IO_FILE *)
  0.00     77.94     0.00        2     0.00     0.00  pIsHomogeneous(spolyrec *)
  0.00     77.94     0.00        2     0.00     0.00  reinit_yylp(void)
  0.00     77.94     0.00        2     0.00     0.14  yylp_create_buffer(_IO_FILE *, int)
  0.00     77.94     0.00        2     0.00     0.15  yylplex(char *, char *, lib_style_types *, lp_modes)
  0.00     77.94     0.00        2     0.00     0.00  yylprestart(_IO_FILE *)
  0.00     77.94     0.00        2     0.00     0.00  yylpwrap
  0.00     77.94     0.00        1     0.00     0.00  IsPrime(int)
  0.00     77.94     0.00        1     0.00     0.00  PrintLn
  0.00     77.94     0.00        1     0.00     0.00  SetProcs(p_Field, p_Length, p_Ord)
  0.00     77.94     0.00        1     0.00     0.00  global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     77.94     0.00        1     0.00     0.00  global destructors keyed to gmp_output_digits
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to MP_INT_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to sattr::Print(void)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to fglmVector::fglmVector(fglmVectorRep *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to idealFunctionals::idealFunctionals(int, int)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to convSingNClapN(snumber *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to dArith2
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to feOptSpec
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to feVersionId
  0.00     77.94     0.00        1     0.00     0.14  global constructors keyed to fe_promptstr
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to fglmUpdatesource(sip_sideal *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to fglmcomb.cc
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to gmp_output_digits
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to iiCurrArgs
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to ip_smatrix_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to jjSYSTEM(sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to kBucketCreate(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to mpsr_sleftv_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to omSingOutOfMemoryFunc
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to rnumber_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to sSubexpr_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to s_si_link_extension_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to singclap_gcd(spolyrec *, spolyrec *)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to sip_command_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to slists_bin
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &)
  0.00     77.94     0.00        1     0.00     0.00  global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &)
  0.00     77.94     0.00        1     0.00     0.00  _fe_getopt_internal
  0.00     77.94     0.00        1     0.00     0.00  _omIsOnList
  0.00     77.94     0.00        1     0.00     0.00  _omRemoveFromList
  0.00     77.94     0.00        1     0.00     0.00  atGet(sleftv *, char *)
  0.00     77.94     0.00        1     0.00    95.97  cleanT(skStrategy *)
  0.00     77.94     0.00        1     0.00    95.97  exitBuchMora(skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  exprlist_length(sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  feGetExpandedExecutable(void)
  0.00     77.94     0.00        1     0.00     0.00  feInitResources(char *)
  0.00     77.94     0.00        1     0.00     0.00  feOptAction(feOptIndex)
  0.00     77.94     0.00        1     0.00     0.00  feSetOptValue(feOptIndex, int)
  0.00     77.94     0.00        1     0.00     0.00  fe_getopt_long
  0.00     77.94     0.00        1     0.00     0.00  full_readlink
  0.00     77.94     0.00        1     0.00     0.44  idCopy(sip_sideal *)
  0.00     77.94     0.00        1     0.00     0.00  idHomIdeal(sip_sideal *, sip_sideal *)
  0.00     77.94     0.00        1     0.00     0.00  idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  idSkipZeroes(sip_sideal *)
  0.00     77.94     0.00        1     0.00     0.00  iiDummy(void *)
  0.00     77.94     0.00        1     0.00     0.00  iiExport(sleftv *, int)
  0.00     77.94     0.00        1     0.00     0.00  iiI2P(void *)
  0.00     77.94     0.00        1     0.00     0.00  initBuchMoraCrit(skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  initBuchMoraPos(skStrategy *)
  0.00     77.94     0.00        1     0.00     3.52  initBuchMora(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  initMora(sip_sideal *, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  initRTimer(void)
  0.00     77.94     0.00        1     0.00     3.23  initSL(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  initTimer(void)
  0.00     77.94     0.00        1     0.00     0.00  init_signals(void)
  0.00     77.94     0.00        1     0.00     0.00  inits(void)
  0.00     77.94     0.00        1     0.00     0.00  jiA_POLY(sleftv *, sleftv *, _ssubexpr *)
  0.00     77.94     0.00        1     0.00     0.00  jjInitTab1(void)
  0.00     77.94     0.00        1     0.00     0.00  jjJACOB_P(sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  jjOPTION_PL(sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00 75313.46  jjSTD(sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  kMoraUseBucket(skStrategy *)
  0.00     77.94     0.00        1     0.00 75313.46  kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *)
  0.00     77.94     0.00        1     0.00     0.00  m2_end
  0.00     77.94     0.00        1     0.00 75359.17  main
  0.00     77.94     0.00        1     0.00     0.00  messageStat(int, int, int, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  mpsr_Init(void)
  0.00     77.94     0.00        1     0.00    10.29  nInitChar(sip_sring *)
  0.00     77.94     0.00        1     0.00    10.29  nSetChar(sip_sring *, short)
  0.00     77.94     0.00        1     0.00     0.00  omFindExec
  0.00     77.94     0.00        1     0.00     0.00  omFindExec_link
  0.00     77.94     0.00        1     0.00     0.00  omFreeKeptAddrFromBin
  0.00     77.94     0.00        1     0.00     0.00  omGetStickyBinOfBin
  0.00     77.94     0.00        1     0.00     0.00  omInitInfo
  0.00     77.94     0.00        1     0.00     0.00  omIsKnownTopBin
  0.00     77.94     0.00        1     0.00     0.00  omMergeStickyBinIntoBin
  0.00     77.94     0.00        1     0.00     0.00  omSizeWOfAddr
  0.00     77.94     0.00        1     0.00     0.00  pIsConstant(spolyrec *)
  0.00     77.94     0.00        1     0.00     0.00  pNormalize(spolyrec *)
  0.00     77.94     0.00        1     0.00     0.00  pSetGlobals(sip_sring *, short)
  0.00     77.94     0.00        1     0.00     0.00  p_FieldIs(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  p_GetSetmProc(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  p_OrdIs(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  p_SetProcs(sip_sring *, p_Procs_s *)
  0.00     77.94     0.00        1     0.00     0.00  pairs(skStrategy *)
  0.00     77.94     0.00        1     0.00    10.30  rComplete(sip_sring *, int)
  0.00     77.94     0.00        1     0.00     0.00  rFieldType(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  rGetDivMask(int)
  0.00     77.94     0.00        1     0.00     0.00  rGetExpSize(unsigned long, int &, int)
  0.00     77.94     0.00        1     0.00    10.30  rInit(sleftv *, sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  rOrd_is_Totaldegree_Ordering(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  rOrderName(char *)
  0.00     77.94     0.00        1     0.00     0.00  rRightAdjustVarOffset(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  rSetDegStuff(sip_sring *)
  0.00     77.94     0.00        1     0.00    10.29  rSetHdl(idrec *, short)
  0.00     77.94     0.00        1     0.00     0.00  rSetNegWeight(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  rSetVarL(sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  rSleftvList2StringArray(sleftv *, char **)
  0.00     77.94     0.00        1     0.00     0.00  rSleftvOrdering2Ordering(sleftv *, sip_sring *)
  0.00     77.94     0.00        1     0.00     0.00  reorderS(int *, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.00  setOption(sleftv *, sleftv *)
  0.00     77.94     0.00        1     0.00     0.00  slInitDBMExtension(s_si_link_extension *)
  0.00     77.94     0.00        1     0.00     0.00  slInitMPFileExtension(s_si_link_extension *)
  0.00     77.94     0.00        1     0.00     0.00  slInitMPTcpExtension(s_si_link_extension *)
  0.00     77.94     0.00        1     0.00     0.01  slStandardInit(void)
  0.00     77.94     0.00        1     0.00     0.00  updateS(short, skStrategy *)
  0.00     77.94     0.00        1     0.00     0.14  yy_create_buffer(_IO_FILE *, int)

 %         the percentage of the total running time of the
time       program used by this function.

cumulative a running sum of the number of seconds accounted
 seconds   for by this function and those listed above it.

 self      the number of seconds accounted for by this
seconds    function alone.  This is the major sort for this
           listing.

calls      the number of times this function was invoked, if
           this function is profiled, else blank.
 
 self      the average number of milliseconds spent in this
ms/call    function per call, if this function is profiled,
	   else blank.

 total     the average number of milliseconds spent in this
ms/call    function and its descendents per call, if this 
	   function is profiled, else blank.

name       the name of the function.  This is the minor sort
           for this listing. The index shows the location of
	   the function in the gprof listing. If the index is
	   in parenthesis it shows where it would appear in
	   the gprof listing if it were to be printed.

		     Call graph (explanation follows)


granularity: each sample hit covers 4 byte(s) for 0.01% of 75.84 seconds

index % time    self  children    called     name
                0.00   75.36       1/1           _start [2]
[1]     99.4    0.00   75.36       1         main [1]
                0.00   75.36       1/1           yyparse(void) <cycle 1> [33]
                0.00    0.00       1/2           iiLibCmd(char *, short) [90]
                0.00    0.00       4/13          feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00       1/1           slStandardInit(void) [126]
                0.00    0.00       2/2           newFile(char *, _IO_FILE *) [127]
                0.00    0.00       1/33          namerec::push(sip_package *, char *, int, short) [124]
                0.00    0.00       1/1           jjInitTab1(void) [311]
                0.00    0.00       1/1           inits(void) [310]
                0.00    0.00       1/1           feInitResources(char *) [293]
                0.00    0.00       1/1           fe_getopt_long [296]
                0.00    0.00       1/85          Print [172]
-----------------------------------------------
                                                 <spontaneous>
[2]     99.4    0.00   75.36                 _start [2]
                0.00   75.36       1/1           main [1]
-----------------------------------------------
[3]     99.4    0.00   75.36       1+473     <cycle 1 as a whole> [3]
                0.00   75.31      67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.04      33             yyparse(void) <cycle 1> [33]
                0.00    0.00      32             iiPStart(idrec *, sleftv *) <cycle 1> [71]
                0.00    0.00     172             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
                0.00    0.00      88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
                0.00    0.00      32             iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
                0.00    0.00      18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [214]
                0.00    0.00      18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [213]
-----------------------------------------------
                                  67             yyparse(void) <cycle 1> [33]
[4]     99.3    0.00   75.31      67         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00   75.31       1/1           jjSTD(sleftv *, sleftv *) [5]
                0.00    0.00      23/74          iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00       1/1           jjJACOB_P(sleftv *, sleftv *) [132]
                0.00    0.00      90/988         sleftv::CleanUp(void) [141]
                0.00    0.00      67/1176        sleftv::Typ(void) [140]
                0.00    0.00      67/239         iiTabIndex(sValCmdTab const *, int, int) [158]
                0.00    0.00      23/79          iiTestConvert(int, int) [174]
                0.00    0.00      18/18          jjCOUNT_L(sleftv *, sleftv *) [211]
                0.00    0.00      16/16          jjDEFINED(sleftv *, sleftv *) [223]
                0.00    0.00       7/575         sleftv::Data(void) [147]
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [244]
                0.00    0.00       3/3           jjMEMORY(sleftv *, sleftv *) [272]
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [231]
-----------------------------------------------
                0.00   75.31       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[5]     99.3    0.00   75.31       1         jjSTD(sleftv *, sleftv *) [5]
                0.00   75.31       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/1           atGet(sleftv *, char *) [290]
                0.00    0.00       1/575         sleftv::Data(void) [147]
                0.00    0.00       1/1           idSkipZeroes(sip_sideal *) [299]
-----------------------------------------------
                0.00   75.31       1/1           jjSTD(sleftv *, sleftv *) [5]
[6]     99.3    0.00   75.31       1         kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.02   75.29       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       1/1           idHomIdeal(sip_sideal *, sip_sideal *) [135]
                0.00    0.00       1/64          __builtin_new [4309]
                0.00    0.00       1/5           sLObject::GetpLength(void) [254]
                0.00    0.00       1/1           omGetStickyBinOfBin [320]
                0.00    0.00       1/1           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [298]
                0.00    0.00       1/1           omMergeStickyBinIntoBin [323]
                0.00    0.00       1/63          __builtin_delete [4310]
-----------------------------------------------
                0.02   75.29       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[7]     99.3    0.02   75.29       1         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
               21.55   53.60    9135/9135        redEcart(sLObject *, skStrategy *) [8]
                0.00    0.10       1/1           exitBuchMora(skStrategy *) [30]
                0.01    0.01    9135/9135        message(int, int *, int *, skStrategy *) [42]
                0.01    0.00      15/820         pNorm(spolyrec *) [16]
                0.00    0.00      18/827         enterT(sLObject, skStrategy *, int) [21]
                0.00    0.00      29/29          ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
                0.00    0.00      18/827         kBucketClear(kBucket *, spolyrec **, int *) [37]
                0.00    0.00      18/18          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [83]
                0.00    0.00       2/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.00    0.00       4/842         kBucketCreate(sip_sring *) [55]
                0.00    0.00      18/9141        PrintS [53]
                0.00    0.00       1/34297       omFreeToPageFault [35]
                0.00    0.00      18/842         kBucketDestroy(kBucket **) [143]
                0.00    0.00      18/18          redtail(spolyrec *, int, skStrategy *) [216]
                0.00    0.00      18/22          cancelunit(sLObject *) [203]
                0.00    0.00      18/18          enterSMora(sLObject, int, skStrategy *, int) [209]
                0.00    0.00      17/17          posInS(spolyrec **, int, spolyrec *) [222]
                0.00    0.00       4/5           pLength(spolyrec *) [257]
                0.00    0.00       4/5           sLObject::GetpLength(void) [254]
                0.00    0.00       4/842         kBucketInit(kBucket *, spolyrec *, int) [144]
                0.00    0.00       1/1           initBuchMoraCrit(skStrategy *) [303]
                0.00    0.00       1/1           initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [305]
                0.00    0.00       1/1           initMora(sip_sideal *, skStrategy *) [306]
                0.00    0.00       1/1           initBuchMoraPos(skStrategy *) [304]
                0.00    0.00       1/1           kMoraUseBucket(skStrategy *) [313]
                0.00    0.00       1/1           messageStat(int, int, int, skStrategy *) [315]
-----------------------------------------------
               21.55   53.60    9135/9135        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[8]     99.1   21.55   53.60    9135         redEcart(sLObject *, skStrategy *) [8]
                0.25   52.05  415695/415695      doRed(sLObject *, sTObject *, short, skStrategy *) [9]
                0.63    0.00  424815/424837      p_GetShortExpVector(spolyrec *, sip_sring *) [15]
                0.28    0.00  415717/415717      kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [19]
                0.23    0.00  233921/234081      posInL17(sLObject *, int, sLObject *, skStrategy *) [20]
                0.14    0.00  415680/415907      pDeg(spolyrec *, sip_sring *) [26]
                0.01    0.00    9098/9098        kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [54]
                0.01    0.00    9102/9263        enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00      18/1654        kBucketCanonicalize(kBucket *) [32]
-----------------------------------------------
                0.25   52.05  415695/415695      redEcart(sLObject *, skStrategy *) [8]
[9]     69.0    0.25   52.05  415695         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
                0.60   50.46  415695/415695      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
                0.37    0.18     801/820         pNorm(spolyrec *) [16]
                0.01    0.19     809/827         enterT(sLObject, skStrategy *, int) [21]
                0.15    0.03    1618/1632        p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.00    0.03     809/1654        kBucketCanonicalize(kBucket *) [32]
                0.00    0.03     809/827         kBucketClear(kBucket *, spolyrec **, int *) [37]
                0.01    0.00     809/842         kBucketCreate(sip_sring *) [55]
                0.00    0.00     809/842         kBucketInit(kBucket *, spolyrec *, int) [144]
                0.00    0.00     809/842         kBucketDestroy(kBucket **) [143]
-----------------------------------------------
                0.60   50.46  415695/415695      doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[10]    67.3    0.60   50.46  415695         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
                0.40   49.32  415681/415710      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
                0.56    0.00  415695/415724      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket [17]
                0.11    0.00  415695/747137      nDummy1(snumber **) [24]
                0.06    0.00  415681/747000      npIsOne(snumber *) [29]
                0.01    0.00    8539/34297       omFreeToPageFault [35]
                0.00    0.00      15/842         kBucketDestroy(kBucket **) [143]
-----------------------------------------------
                0.00    0.00      29/415710      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.40   49.32  415681/415710      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
[11]    65.6    0.40   49.32  415710         kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
               47.70    0.40  312360/312360      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [12]
                0.88    0.01  101116/107354      p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [13]
                0.29    0.04  103350/205503      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [14]
                0.01    0.00  206700/206700      npNeg(snumber *) [52]
-----------------------------------------------
               47.70    0.40  312360/312360      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
[12]    63.4   47.70    0.40  312360         p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [12]
                0.28    0.04  102124/205503      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [14]
                0.03    0.04   11218/32070       omAllocBinFromFullPage [23]
                0.00    0.00    3186/34297       omFreeToPageFault [35]
-----------------------------------------------
                0.00    0.00       7/107354      jjPLUS_P(sleftv *, sleftv *, sleftv *) [108]
                0.05    0.00    6231/107354      kBucketCanonicalize(kBucket *) [32]
                0.88    0.01  101116/107354      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
[13]     1.2    0.93    0.01  107354         p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [13]
                0.01    0.00   12166/34297       omFreeToPageFault [35]
-----------------------------------------------
                0.00    0.00      29/205503      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.28    0.04  102124/205503      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [12]
                0.29    0.04  103350/205503      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
[14]     0.9    0.57    0.08  205503         pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [14]
                0.04    0.04   13947/32070       omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       4/424837      initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00      18/424837      enterpairs(spolyrec *, int, int, int, skStrategy *, int) [83]
                0.63    0.00  424815/424837      redEcart(sLObject *, skStrategy *) [8]
[15]     0.8    0.63    0.00  424837         p_GetShortExpVector(spolyrec *, sip_sring *) [15]
-----------------------------------------------
                0.00    0.00       4/820         initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.01    0.00      15/820         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.37    0.18     801/820         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[16]     0.7    0.38    0.19     820         pNorm(spolyrec *) [16]
                0.08    0.00  331226/747137      nDummy1(snumber **) [24]
                0.04    0.00  331227/747000      npIsOne(snumber *) [29]
                0.04    0.00  330407/330407      npDiv(snumber *, snumber *) [34]
                0.02    0.00  331218/331218      nDummy2(snumber *&) [38]
                0.00    0.00     819/875         npInit(int) [142]
-----------------------------------------------
                0.00    0.00      29/415724      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.56    0.00  415695/415724      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
[17]     0.7    0.56    0.00  415724         p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket [17]
                0.00    0.00    2847/34297       omFreeToPageFault [35]
-----------------------------------------------
                                                 <spontaneous>
[18]     0.4    0.33    0.00                 write [18]
-----------------------------------------------
                0.28    0.00  415717/415717      redEcart(sLObject *, skStrategy *) [8]
[19]     0.4    0.28    0.00  415717         kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [19]
-----------------------------------------------
                0.00    0.00       3/234081      initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00      50/234081      chainCrit(spolyrec *, int, skStrategy *) [109]
                0.00    0.00     107/234081      enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
                0.23    0.00  233921/234081      redEcart(sLObject *, skStrategy *) [8]
[20]     0.3    0.23    0.00  234081         posInL17(sLObject *, int, sLObject *, skStrategy *) [20]
-----------------------------------------------
                0.00    0.00      18/827         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.01    0.19     809/827         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[21]     0.3    0.01    0.20     827         enterT(sLObject, skStrategy *, int) [21]
                0.18    0.02     826/826         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [22]
                0.00    0.00     125/126         omDoRealloc [85]
                0.00    0.00       3/32070       omAllocBinFromFullPage [23]
                0.00    0.00       2/34297       omFreeToPageFault [35]
                0.00    0.00     827/827         posInT17(sTObject *, int, sLObject const &) [145]
-----------------------------------------------
                0.18    0.02     826/826         enterT(sLObject, skStrategy *, int) [21]
[22]     0.3    0.18    0.02     826         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [22]
                0.01    0.01    1978/32070       omAllocBinFromFullPage [23]
                0.00    0.00    3487/34297       omFreeToPageFault [35]
-----------------------------------------------
                0.00    0.00       1/32070       idrec::set(char *, int, int, short) [123]
                0.00    0.00       1/32070       namerec::push(sip_package *, char *, int, short) [124]
                0.00    0.00       1/32070       iiCheckNest(void) [125]
                0.00    0.00       1/32070       rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00       1/32070       rComplete(sip_sring *, int) [45]
                0.00    0.00       1/32070       slStandardInit(void) [126]
                0.00    0.00       2/32070       iiGetLibProcBuffer(procinfo *, int) [115]
                0.00    0.00       3/32070       enterT(sLObject, skStrategy *, int) [21]
                0.00    0.00       5/32070       feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00      18/32070       __builtin_vec_new [99]
                0.01    0.01    1978/32070       p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [22]
                0.01    0.02    4893/32070       p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.03    0.04   11218/32070       p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [12]
                0.04    0.04   13947/32070       pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [14]
[23]     0.3    0.09    0.10   32070         omAllocBinFromFullPage [23]
                0.10    0.00    2301/2301        omAllocBinPage [28]
-----------------------------------------------
                0.00    0.00       1/747137      exitBuchMora(skStrategy *) [30]
                0.00    0.00      11/747137      pDiff(spolyrec *, int) [131]
                0.00    0.00      29/747137      ksCheckCoeff(snumber **, snumber **) [118]
                0.00    0.00      58/747137      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00     117/747137      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
                0.08    0.00  331226/747137      pNorm(spolyrec *) [16]
                0.11    0.00  415695/747137      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
[24]     0.3    0.19    0.00  747137         nDummy1(snumber **) [24]
-----------------------------------------------
                0.00    0.00       1/1632        slInternalCopy(sleftv *, int, void *, _ssubexpr *) [81]
                0.00    0.00       4/1632        idCopy(sip_sideal *) [84]
                0.00    0.00       4/1632        initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00       5/1632        pPower(spolyrec *, int) [75]
                0.15    0.03    1618/1632        doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[25]     0.2    0.15    0.03    1632         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.01    0.02    4893/32070       omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       4/415907      initEcartNormal(sLObject *) [133]
                0.00    0.00       4/415907      pLDeg0c(spolyrec *, int *, sip_sring *) [137]
                0.00    0.00       5/415907      pIsHomogeneous(spolyrec *) [134]
                0.00    0.00     214/415907      initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.14    0.00  415680/415907      redEcart(sLObject *, skStrategy *) [8]
[26]     0.2    0.14    0.00  415907         pDeg(spolyrec *, sip_sring *) [26]
-----------------------------------------------
                0.00    0.00       2/885         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       6/885         pPower(spolyrec *, int) [75]
                0.00    0.00       7/885         jjTIMES_P(sleftv *, sleftv *, sleftv *) [77]
                0.00    0.00      10/885         id_Delete(sip_sideal **, sip_sring *) [76]
                0.01    0.00      49/885         pmInit(char *, short &) [63]
                0.09    0.00     811/885         cleanT(skStrategy *) [31]
[27]     0.1    0.10    0.00     885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.00    0.00    4052/34297       omFreeToPageFault [35]
-----------------------------------------------
                0.10    0.00    2301/2301        omAllocBinFromFullPage [23]
[28]     0.1    0.10    0.00    2301         omAllocBinPage [28]
                0.00    0.00      17/17          omAllocNewBinPagesRegion [70]
-----------------------------------------------
                0.00    0.00       5/747000      pPower(spolyrec *, int) [75]
                0.00    0.00      87/747000      ksCheckCoeff(snumber **, snumber **) [118]
                0.04    0.00  331227/747000      pNorm(spolyrec *) [16]
                0.06    0.00  415681/747000      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
[29]     0.1    0.10    0.00  747000         npIsOne(snumber *) [29]
-----------------------------------------------
                0.00    0.10       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[30]     0.1    0.00    0.10       1         exitBuchMora(skStrategy *) [30]
                0.00    0.10       1/1           cleanT(skStrategy *) [31]
                0.00    0.00       1/747137      nDummy1(snumber **) [24]
                0.00    0.00       5/338         omSizeOfLargeAddr [156]
                0.00    0.00       5/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.10       1/1           exitBuchMora(skStrategy *) [30]
[31]     0.1    0.00    0.10       1         cleanT(skStrategy *) [31]
                0.09    0.00     811/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
-----------------------------------------------
                0.00    0.00      18/1654        redEcart(sLObject *, skStrategy *) [8]
                0.00    0.03     809/1654        doRed(sLObject *, sTObject *, short, skStrategy *) [9]
                0.00    0.03     827/1654        kBucketClear(kBucket *, spolyrec **, int *) [37]
[32]     0.1    0.00    0.05    1654         kBucketCanonicalize(kBucket *) [32]
                0.05    0.00    6231/107354      p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [13]
-----------------------------------------------
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [71]
                0.00   75.36       1/1           main [1]
[33]     0.1    0.00    0.04      33         yyparse(void) <cycle 1> [33]
                0.01    0.00    1822/1822        yylex(MYYSTYPE *) [43]
                0.00    0.01       1/1           rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.01       1/1           rSetHdl(idrec *, short) [50]
                0.00    0.01     330/330         syMake(sleftv *, char *, idrec *) [62]
                0.00    0.00      28/60          newBuffer(char *, feBufferTypes, procinfo *, int) [65]
                0.00    0.00      67/106         iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00      45/45          iiParameter(sleftv *) [82]
                0.00    0.00       1/2           iiLibCmd(char *, short) [90]
                0.00    0.00      26/26          sleftv::Print(sleftv *, int) [128]
                0.00    0.00      79/79          iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [130]
                0.00    0.00       1/122         enterid(char *, int, int, idrec **, short) [122]
                0.00    0.00      76/1176        sleftv::Typ(void) [140]
                0.00    0.00      63/988         sleftv::CleanUp(void) [141]
                0.00    0.00      51/575         sleftv::Data(void) [147]
                0.00    0.00      32/32          exitBuffer(feBufferTypes) [191]
                0.00    0.00      20/32          sleftv::Copy(sleftv *) [190]
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00       1/1           rOrderName(char *) [337]
                0.00    0.00       1/64          __builtin_new [4309]
                0.00    0.00       1/1           iiExport(sleftv *, int) [301]
                                 154             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
                                  88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
                                  67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
-----------------------------------------------
                0.04    0.00  330407/330407      pNorm(spolyrec *) [16]
[34]     0.1    0.04    0.00  330407         npDiv(snumber *, snumber *) [34]
-----------------------------------------------
                0.00    0.00       1/34297       mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       1/34297       rComplete(sip_sring *, int) [45]
                0.00    0.00       1/34297       omDoRealloc [85]
                0.00    0.00       2/34297       enterT(sLObject, skStrategy *, int) [21]
                0.00    0.00       5/34297       feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00      10/34297       ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00    2847/34297       p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket [17]
                0.00    0.00    3186/34297       p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [12]
                0.00    0.00    3487/34297       p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [22]
                0.00    0.00    4052/34297       p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.01    0.00    8539/34297       ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
                0.01    0.00   12166/34297       p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [13]
[35]     0.1    0.04    0.00   34297         omFreeToPageFault [35]
                0.00    0.00    2262/2262        omFreeBinPages [139]
-----------------------------------------------
                                                 <spontaneous>
[36]     0.1    0.04    0.00                 memmove [36]
-----------------------------------------------
                0.00    0.00      18/827         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.03     809/827         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[37]     0.0    0.00    0.03     827         kBucketClear(kBucket *, spolyrec **, int *) [37]
                0.00    0.03     827/1654        kBucketCanonicalize(kBucket *) [32]
-----------------------------------------------
                0.02    0.00  331218/331218      pNorm(spolyrec *) [16]
[38]     0.0    0.02    0.00  331218         nDummy2(snumber *&) [38]
-----------------------------------------------
                                                 <spontaneous>
[39]     0.0    0.02    0.00                 fork [39]
-----------------------------------------------
                                                 <spontaneous>
[40]     0.0    0.02    0.00                 fwrite [40]
-----------------------------------------------
                                                 <spontaneous>
[41]     0.0    0.02    0.00                 _IO_file_xsputn [41]
-----------------------------------------------
                0.01    0.01    9135/9135        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[42]     0.0    0.01    0.01    9135         message(int, int *, int *, skStrategy *) [42]
                0.01    0.00    9123/9141        PrintS [53]
                0.00    0.00      81/85          Print [172]
-----------------------------------------------
                0.01    0.00    1822/1822        yyparse(void) <cycle 1> [33]
[43]     0.0    0.01    0.00    1822         yylex(MYYSTYPE *) [43]
                0.00    0.00     790/790         yy_get_next_buffer(void) [94]
                0.00    0.00       1/1           yy_create_buffer(_IO_FILE *, int) [103]
                0.00    0.00     774/774         yy_get_previous_state(void) [146]
                0.00    0.00     512/512         IsCmd(char *, int &) [149]
                0.00    0.00      16/61          exitVoice(void) [180]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [197]
                0.00    0.00       1/97          yy_load_buffer_state(void) [168]
                0.00    0.00       1/1           m2_end [314]
-----------------------------------------------
                0.00    0.01       1/1           yyparse(void) <cycle 1> [33]
[44]     0.0    0.00    0.01       1         rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.01       1/1           rComplete(sip_sring *, int) [45]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
                0.00    0.00       2/988         sleftv::CleanUp(void) [141]
                0.00    0.00       1/1176        sleftv::Typ(void) [140]
                0.00    0.00       1/575         sleftv::Data(void) [147]
                0.00    0.00       1/1           IsPrime(int) [287]
                0.00    0.00       1/346         sleftv::listLength(void) [154]
                0.00    0.00       1/1           rSleftvList2StringArray(sleftv *, char **) [342]
                0.00    0.00       1/1           rSleftvOrdering2Ordering(sleftv *, sip_sring *) [343]
                0.00    0.00       1/3           rDBTest(sip_sring *, char *, int) [276]
-----------------------------------------------
                0.00    0.01       1/1           rInit(sleftv *, sleftv *, sleftv *) [44]
[45]     0.0    0.00    0.01       1         rComplete(sip_sring *, int) [45]
                0.00    0.01       1/1           nInitChar(sip_sring *) [48]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
                0.00    0.00       1/34297       omFreeToPageFault [35]
                0.00    0.00       1/1           omSizeWOfAddr [324]
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [339]
                0.00    0.00       1/1           rGetExpSize(unsigned long, int &, int) [335]
                0.00    0.00       1/1           rGetDivMask(int) [334]
                0.00    0.00       1/42          _omGetSpecBin [4311]
                0.00    0.00       1/1           p_GetSetmProc(sip_sring *) [329]
                0.00    0.00       1/1           rSetVarL(sip_sring *) [341]
                0.00    0.00       1/1           rRightAdjustVarOffset(sip_sring *) [338]
                0.00    0.00       1/1           rSetNegWeight(sip_sring *) [340]
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [331]
-----------------------------------------------
                0.01    0.00       1/1           nInitChar(sip_sring *) [48]
[46]     0.0    0.01    0.00       1         npInitChar(int, sip_sring *) [46]
                0.00    0.00       2/69          omAllocFromSystem [56]
-----------------------------------------------
                0.01    0.00       1/1           nSetChar(sip_sring *, short) [49]
[47]     0.0    0.01    0.00       1         npSetChar(int, sip_sring *) [47]
                0.00    0.00       2/69          omAllocFromSystem [56]
-----------------------------------------------
                0.00    0.01       1/1           rComplete(sip_sring *, int) [45]
[48]     0.0    0.00    0.01       1         nInitChar(sip_sring *) [48]
                0.01    0.00       1/1           npInitChar(int, sip_sring *) [46]
                0.00    0.00       1/1           rFieldType(sip_sring *) [333]
                0.00    0.00       1/875         npInit(int) [142]
-----------------------------------------------
                0.00    0.01       1/1           rSetHdl(idrec *, short) [50]
[49]     0.0    0.00    0.01       1         nSetChar(sip_sring *, short) [49]
                0.01    0.00       1/1           npSetChar(int, sip_sring *) [47]
-----------------------------------------------
                0.00    0.01       1/1           yyparse(void) <cycle 1> [33]
[50]     0.0    0.00    0.01       1         rSetHdl(idrec *, short) [50]
                0.00    0.01       1/1           nSetChar(sip_sring *, short) [49]
                0.00    0.00       2/3           rDBTest(sip_sring *, char *, int) [276]
                0.00    0.00       1/1           pSetGlobals(sip_sring *, short) [327]
-----------------------------------------------
                0.00    0.00       4/9263        initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00      50/9263        chainCrit(spolyrec *, int, skStrategy *) [109]
                0.00    0.00     107/9263        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
                0.01    0.00    9102/9263        redEcart(sLObject *, skStrategy *) [8]
[51]     0.0    0.01    0.00    9263         enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00       1/126         omDoRealloc [85]
-----------------------------------------------
                0.01    0.00  206700/206700      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
[52]     0.0    0.01    0.00  206700         npNeg(snumber *) [52]
-----------------------------------------------
                0.00    0.00      18/9141        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.01    0.00    9123/9141        message(int, int *, int *, skStrategy *) [42]
[53]     0.0    0.01    0.00    9141         PrintS [53]
-----------------------------------------------
                0.01    0.00    9098/9098        redEcart(sLObject *, skStrategy *) [8]
[54]     0.0    0.01    0.00    9098         kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [54]
-----------------------------------------------
                0.00    0.00       4/842         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      29/842         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.01    0.00     809/842         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[55]     0.0    0.01    0.00     842         kBucketCreate(sip_sring *) [55]
-----------------------------------------------
                0.00    0.00       1/69          global constructors keyed to fe_promptstr [104]
                0.00    0.00       1/69          omBinPageIndexFault [101]
                0.00    0.00       1/69          __builtin_vec_new [99]
                0.00    0.00       2/69          feReadLine(char *, int) [93]
                0.00    0.00       2/69          initL(void) [96]
                0.00    0.00       2/69          npSetChar(int, sip_sring *) [47]
                0.00    0.00       2/69          npInitChar(int, sip_sring *) [46]
                0.00    0.00       2/69          yy_flex_alloc(unsigned int) [95]
                0.00    0.00       3/69          malloc [86]
                0.00    0.00       3/69          omDoRealloc [85]
                0.00    0.00      17/69          omAllocNewBinPagesRegion [70]
                0.00    0.00      33/69          yy_flex_alloc(unsigned int) [64]
[56]     0.0    0.01    0.00      69         omAllocFromSystem [56]
                0.00    0.00      69/82          mALLOc [173]
-----------------------------------------------
                                                 <spontaneous>
[57]     0.0    0.01    0.00                 mmap [57]
-----------------------------------------------
                                                 <spontaneous>
[58]     0.0    0.01    0.00                 _IO_do_write [58]
-----------------------------------------------
                                                 <spontaneous>
[59]     0.0    0.01    0.00                 _IO_file_sync [59]
-----------------------------------------------
                                                 <spontaneous>
[60]     0.0    0.01    0.00                 _IO_file_write [60]
-----------------------------------------------
                                                 <spontaneous>
[61]     0.0    0.01    0.00                 _IO_str_init_static [61]
-----------------------------------------------
                0.00    0.01     330/330         yyparse(void) <cycle 1> [33]
[62]     0.0    0.00    0.01     330         syMake(sleftv *, char *, idrec *) [62]
                0.00    0.01      49/49          pmInit(char *, short &) [63]
                0.00    0.00     330/330         ggetid(char const *, short) [157]
                0.00    0.00      23/125         rIsRingVar(char *) [165]
                0.00    0.00      14/16          p_ISet(int, sip_sring *) [227]
                0.00    0.00      14/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
-----------------------------------------------
                0.00    0.01      49/49          syMake(sleftv *, char *, idrec *) [62]
[63]     0.0    0.00    0.01      49         pmInit(char *, short &) [63]
                0.01    0.00      49/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.00    0.00     102/125         rIsRingVar(char *) [165]
                0.00    0.00      49/49          npRead(char *, snumber **) [183]
                0.00    0.00       4/4           eati(char *, int *) [259]
-----------------------------------------------
                0.00    0.00       2/66          yy_create_buffer(_IO_FILE *, int) [103]
                0.00    0.00      64/66          myynewbuffer(void) [66]
[64]     0.0    0.00    0.00      66         yy_flex_alloc(unsigned int) [64]
                0.00    0.00      33/69          omAllocFromSystem [56]
-----------------------------------------------
                0.00    0.00      28/60          yyparse(void) <cycle 1> [33]
                0.00    0.00      32/60          iiPStart(idrec *, sleftv *) <cycle 1> [71]
[65]     0.0    0.00    0.00      60         newBuffer(char *, feBufferTypes, procinfo *, int) [65]
                0.00    0.00      32/32          myynewbuffer(void) [66]
                0.00    0.00      60/64          __builtin_new [4309]
-----------------------------------------------
                0.00    0.00      32/32          newBuffer(char *, feBufferTypes, procinfo *, int) [65]
[66]     0.0    0.00    0.00      32         myynewbuffer(void) [66]
                0.00    0.00      64/66          yy_flex_alloc(unsigned int) [64]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [178]
                0.00    0.00      32/97          yy_load_buffer_state(void) [168]
-----------------------------------------------
                0.00    0.00      29/29          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[67]     0.0    0.00    0.00      29         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00      29/415710      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [11]
                0.00    0.00      29/842         kBucketCreate(sip_sring *) [55]
                0.00    0.00      29/205503      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [14]
                0.00    0.00      29/415724      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket [17]
                0.00    0.00      29/29          ksCheckCoeff(snumber **, snumber **) [118]
                0.00    0.00      58/747137      nDummy1(snumber **) [24]
                0.00    0.00      10/34297       omFreeToPageFault [35]
                0.00    0.00      58/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
                0.00    0.00      29/842         kBucketInit(kBucket *, spolyrec *, int) [144]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[68]     0.0    0.00    0.00       1         initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
                0.00    0.00       1/1           initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00       2/2           initL(void) [96]
                0.00    0.00       1/1           updateS(short, skStrategy *) [349]
                0.00    0.00       1/1           pairs(skStrategy *) [332]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
[69]     0.0    0.00    0.00       1         initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00       4/820         pNorm(spolyrec *) [16]
                0.00    0.00       4/1632        p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.00    0.00       4/424837      p_GetShortExpVector(spolyrec *, sip_sring *) [15]
                0.00    0.00       4/9263        enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00       3/234081      posInL17(sLObject *, int, sLObject *, skStrategy *) [20]
                0.00    0.00       4/4           initEcartNormal(sLObject *) [133]
                0.00    0.00       4/22          cancelunit(sLObject *) [203]
                0.00    0.00       4/4           deleteHC(sLObject *, skStrategy *, short) [258]
                0.00    0.00       1/6           idInit(int, int) [252]
                0.00    0.00       1/1           pIsConstant(spolyrec *) [325]
-----------------------------------------------
                0.00    0.00      17/17          omAllocBinPage [28]
[70]     0.0    0.00    0.00      17         omAllocNewBinPagesRegion [70]
                0.00    0.00      17/69          omAllocFromSystem [56]
                0.00    0.00      17/17          omRegisterBinPages [102]
                0.00    0.00      17/17          _omVallocFromSystem [4312]
-----------------------------------------------
                                  32             iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
[71]     0.0    0.00    0.00      32         iiPStart(idrec *, sleftv *) <cycle 1> [71]
                0.00    0.00      32/60          newBuffer(char *, feBufferTypes, procinfo *, int) [65]
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [115]
                0.00    0.00      32/32          killlocals(int) [194]
                                  32             yyparse(void) <cycle 1> [33]
-----------------------------------------------
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [213]
                                 154             yyparse(void) <cycle 1> [33]
[72]     0.0    0.00    0.00     172         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
                0.00    0.00       7/7           jjTIMES_P(sleftv *, sleftv *, sleftv *) [77]
                0.00    0.00       7/7           jjPLUS_P(sleftv *, sleftv *, sleftv *) [108]
                0.00    0.00      38/74          iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00     382/988         sleftv::CleanUp(void) [141]
                0.00    0.00     344/1176        sleftv::Typ(void) [140]
                0.00    0.00     172/239         iiTabIndex(sValCmdTab const *, int, int) [158]
                0.00    0.00      90/90          jjPLUS_S(sleftv *, sleftv *, sleftv *) [170]
                0.00    0.00      42/79          iiTestConvert(int, int) [174]
                0.00    0.00      18/18          jjGT_I(sleftv *, sleftv *, sleftv *) [212]
                0.00    0.00      14/14          jjMINUS_I(sleftv *, sleftv *, sleftv *) [229]
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [262]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [214]
-----------------------------------------------
                0.00    0.00      39/106         iiParameter(sleftv *) [82]
                0.00    0.00      67/106         yyparse(void) <cycle 1> [33]
[73]     0.0    0.00    0.00     106         iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00      93/93          jiAssign_1(sleftv *, sleftv *) [78]
                0.00    0.00       2/4           id_Delete(sip_sideal **, sip_sring *) [76]
                0.00    0.00       2/133         sleftv::CopyD(int) [80]
                0.00    0.00       2/74          iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00     214/1176        sleftv::Typ(void) [140]
                0.00    0.00     212/346         sleftv::listLength(void) [154]
                0.00    0.00     201/988         sleftv::CleanUp(void) [141]
                0.00    0.00      20/32          ipMoveId(idrec *) [193]
                0.00    0.00      12/12          jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00       3/79          iiTestConvert(int, int) [174]
                0.00    0.00       1/575         sleftv::Data(void) [147]
                0.00    0.00       1/1           exprlist_length(sleftv *) [291]
                0.00    0.00       1/6           idInit(int, int) [252]
                0.00    0.00       1/94          sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00      14/14          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[74]     0.0    0.00    0.00      14         jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
                0.00    0.00      14/14          pPower(spolyrec *, int) [75]
                0.00    0.00      14/133         sleftv::CopyD(int) [80]
                0.00    0.00      14/575         sleftv::Data(void) [147]
                0.00    0.00      14/14          jjOP_REST(sleftv *, sleftv *, sleftv *) [230]
-----------------------------------------------
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
[75]     0.0    0.00    0.00      14         pPower(spolyrec *, int) [75]
                0.00    0.00       6/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.00    0.00       5/1632        p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.00    0.00       5/747000      npIsOne(snumber *) [29]
                0.00    0.00       5/11          p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [234]
                0.00    0.00       5/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
                0.00    0.00       1/16          p_ISet(int, sip_sring *) [227]
-----------------------------------------------
                0.00    0.00       2/4           iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00       2/4           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [79]
[76]     0.0    0.00    0.00       4         id_Delete(sip_sideal **, sip_sring *) [76]
                0.00    0.00      10/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[77]     0.0    0.00    0.00       7         jjTIMES_P(sleftv *, sleftv *, sleftv *) [77]
                0.00    0.00       7/885         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [27]
                0.00    0.00      14/133         sleftv::CopyD(int) [80]
                0.00    0.00       6/11          p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [234]
-----------------------------------------------
                0.00    0.00      93/93          iiAssign(sleftv *, sleftv *) [73]
[78]     0.0    0.00    0.00      93         jiAssign_1(sleftv *, sleftv *) [78]
                0.00    0.00       2/2           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [79]
                0.00    0.00      65/65          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [98]
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [129]
                0.00    0.00     186/1176        sleftv::Typ(void) [140]
                0.00    0.00      25/25          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [201]
-----------------------------------------------
                0.00    0.00       2/2           jiAssign_1(sleftv *, sleftv *) [78]
[79]     0.0    0.00    0.00       2         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [79]
                0.00    0.00       2/4           id_Delete(sip_sideal **, sip_sring *) [76]
                0.00    0.00       2/133         sleftv::CopyD(int) [80]
                0.00    0.00       2/94          sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00       1/133         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [129]
                0.00    0.00       1/133         sleftv::Print(sleftv *, int) [128]
                0.00    0.00       2/133         iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00       2/133         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [79]
                0.00    0.00      14/133         jjTIMES_P(sleftv *, sleftv *, sleftv *) [77]
                0.00    0.00      14/133         jjPLUS_P(sleftv *, sleftv *, sleftv *) [108]
                0.00    0.00      14/133         jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
                0.00    0.00      20/133         iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00      65/133         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [98]
[80]     0.0    0.00    0.00     133         sleftv::CopyD(int) [80]
                0.00    0.00       9/9           slInternalCopy(sleftv *, int, void *, _ssubexpr *) [81]
                0.00    0.00     123/560         iiCheckRing(int) [148]
                0.00    0.00      10/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       9/9           sleftv::CopyD(int) [80]
[81]     0.0    0.00    0.00       9         slInternalCopy(sleftv *, int, void *, _ssubexpr *) [81]
                0.00    0.00       1/1           idCopy(sip_sideal *) [84]
                0.00    0.00       1/1632        p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
-----------------------------------------------
                0.00    0.00      45/45          yyparse(void) <cycle 1> [33]
[82]     0.0    0.00    0.00      45         iiParameter(sleftv *) [82]
                0.00    0.00      39/106         iiAssign(sleftv *, sleftv *) [73]
-----------------------------------------------
                0.00    0.00      18/18          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[83]     0.0    0.00    0.00      18         enterpairs(spolyrec *, int, int, int, skStrategy *, int) [83]
                0.00    0.00      18/18          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [87]
                0.00    0.00      18/424837      p_GetShortExpVector(spolyrec *, sip_sring *) [15]
-----------------------------------------------
                0.00    0.00       1/1           slInternalCopy(sleftv *, int, void *, _ssubexpr *) [81]
[84]     0.0    0.00    0.00       1         idCopy(sip_sideal *) [84]
                0.00    0.00       4/1632        p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [25]
                0.00    0.00       1/6           idInit(int, int) [252]
-----------------------------------------------
                0.00    0.00       1/126         enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00     125/126         enterT(sLObject, skStrategy *, int) [21]
[85]     0.0    0.00    0.00     126         omDoRealloc [85]
                0.00    0.00       3/69          omAllocFromSystem [56]
                0.00    0.00       1/34297       omFreeToPageFault [35]
                0.00    0.00      86/86          omRealloc0Large [171]
                0.00    0.00      37/338         omSizeOfLargeAddr [156]
                0.00    0.00      37/139         omReallocSizeFromSystem [163]
                0.00    0.00       6/6           omSizeOfAddr [253]
-----------------------------------------------
                0.00    0.00       2/36          __gmpf_init_set_d [116]
                0.00    0.00       2/36          __gmpz_init_set_si [117]
                0.00    0.00       3/36          opendir [112]
                0.00    0.00      29/36          fopen [88]
[86]     0.0    0.00    0.00      36         malloc [86]
                0.00    0.00       3/69          omAllocFromSystem [56]
-----------------------------------------------
                0.00    0.00      18/18          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [83]
[87]     0.0    0.00    0.00      18         initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [87]
                0.00    0.00     141/141         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
                0.00    0.00      18/18          chainCrit(spolyrec *, int, skStrategy *) [109]
-----------------------------------------------
                                                 <spontaneous>
[88]     0.0    0.00    0.00                 fopen [88]
                0.00    0.00      29/36          malloc [86]
-----------------------------------------------
                0.00    0.00     141/141         initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [87]
[89]     0.0    0.00    0.00     141         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
                0.00    0.00     107/9263        enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00     107/234081      posInL17(sLObject *, int, sLObject *, skStrategy *) [20]
                0.00    0.00     107/107         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.00    0.00     107/107         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
                0.00    0.00     141/141         pLcm(spolyrec *, spolyrec *, spolyrec *) [162]
                0.00    0.00     141/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
                0.00    0.00     141/141         pHasNotCF(spolyrec *, spolyrec *) [161]
                0.00    0.00      57/78          deleteInL(sLObject *, int *, int, skStrategy *) [175]
-----------------------------------------------
                0.00    0.00       1/2           main [1]
                0.00    0.00       1/2           yyparse(void) <cycle 1> [33]
[90]     0.0    0.00    0.00       2         iiLibCmd(char *, short) [90]
                0.00    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [91]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00       1/122         enterid(char *, int, int, idrec **, short) [122]
                0.00    0.00       2/4           idrec::get(char const *, int) [261]
-----------------------------------------------
                0.00    0.00       2/2           iiLibCmd(char *, short) [90]
[91]     0.0    0.00    0.00       2         iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [91]
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
                0.00    0.00       2/2           reinit_yylp(void) [284]
                0.00    0.00       1/85          Print [172]
-----------------------------------------------
                0.00    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [91]
[92]     0.0    0.00    0.00       2         yylplex(char *, char *, lib_style_types *, lp_modes) [92]
                0.00    0.00       2/2           yylp_create_buffer(_IO_FILE *, int) [97]
                0.00    0.00      41/122         enterid(char *, int, int, idrec **, short) [122]
                0.00    0.00     403/403         current_pos(int) [151]
                0.00    0.00     347/347         copy_string(lp_modes) [153]
                0.00    0.00      41/41          iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [186]
                0.00    0.00       9/9           yy_get_next_buffer(void) [239]
                0.00    0.00       7/7           yy_get_previous_state(void) [248]
                0.00    0.00       4/4           make_version(char *, int) [264]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [270]
                0.00    0.00       2/2           yylpwrap [286]
-----------------------------------------------
                0.00    0.00     790/790         yy_get_next_buffer(void) [94]
[93]     0.0    0.00    0.00     790         feReadLine(char *, int) [93]
                0.00    0.00       2/69          omAllocFromSystem [56]
                0.00    0.00     379/379         fePrintEcho(char *, char *) [152]
-----------------------------------------------
                0.00    0.00     790/790         yylex(MYYSTYPE *) [43]
[94]     0.0    0.00    0.00     790         yy_get_next_buffer(void) [94]
                0.00    0.00     790/790         feReadLine(char *, int) [93]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [197]
-----------------------------------------------
                0.00    0.00       4/4           yylp_create_buffer(_IO_FILE *, int) [97]
[95]     0.0    0.00    0.00       4         yy_flex_alloc(unsigned int) [95]
                0.00    0.00       2/69          omAllocFromSystem [56]
-----------------------------------------------
                0.00    0.00       2/2           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
[96]     0.0    0.00    0.00       2         initL(void) [96]
                0.00    0.00       2/69          omAllocFromSystem [56]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[97]     0.0    0.00    0.00       2         yylp_create_buffer(_IO_FILE *, int) [97]
                0.00    0.00       4/4           yy_flex_alloc(unsigned int) [95]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [269]
-----------------------------------------------
                0.00    0.00      65/65          jiAssign_1(sleftv *, sleftv *) [78]
[98]     0.0    0.00    0.00      65         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [98]
                0.00    0.00      65/133         sleftv::CopyD(int) [80]
                0.00    0.00      65/94          sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00       1/167         global constructors keyed to ff_prime [136]
                0.00    0.00       2/167         initPT(void) [100]
                0.00    0.00      82/167         Array<CanonicalForm>::Array(int) [107]
                0.00    0.00      82/167         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [106]
[99]     0.0    0.00    0.00     167         __builtin_vec_new [99]
                0.00    0.00       1/69          omAllocFromSystem [56]
                0.00    0.00      18/32070       omAllocBinFromFullPage [23]
-----------------------------------------------
                                                 <spontaneous>
[100]    0.0    0.00    0.00                 initPT(void) [100]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(int) [107]
                0.00    0.00      82/82          Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [106]
                0.00    0.00       2/167         __builtin_vec_new [99]
                0.00    0.00     156/156         Array<CanonicalForm>::operator[](int) const [4305]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(void) [4308]
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4306]
-----------------------------------------------
                0.00    0.00      17/17          omRegisterBinPages [102]
[101]    0.0    0.00    0.00      17         omBinPageIndexFault [101]
                0.00    0.00       1/69          omAllocFromSystem [56]
                0.00    0.00      16/139         omReallocSizeFromSystem [163]
-----------------------------------------------
                0.00    0.00      17/17          omAllocNewBinPagesRegion [70]
[102]    0.0    0.00    0.00      17         omRegisterBinPages [102]
                0.00    0.00      17/17          omBinPageIndexFault [101]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [43]
[103]    0.0    0.00    0.00       1         yy_create_buffer(_IO_FILE *, int) [103]
                0.00    0.00       2/66          yy_flex_alloc(unsigned int) [64]
                0.00    0.00       1/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [178]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[104]    0.0    0.00    0.00       1         global constructors keyed to fe_promptstr [104]
                0.00    0.00       1/69          omAllocFromSystem [56]
-----------------------------------------------
                                                 <spontaneous>
[105]    0.0    0.00    0.00                 __do_global_ctors_aux [105]
                0.00    0.00       1/1           global constructors keyed to fe_promptstr [104]
                0.00    0.00       1/1           global constructors keyed to feVersionId [4325]
                0.00    0.00       1/1           global constructors keyed to dArith2 [4323]
                0.00    0.00       1/1           global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4342]
                0.00    0.00       1/1           global constructors keyed to sattr::Print(void) [4318]
                0.00    0.00       1/1           global constructors keyed to convSingNClapN(snumber *) [4322]
                0.00    0.00       1/1           global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4338]
                0.00    0.00       1/1           global constructors keyed to omSingOutOfMemoryFunc [4334]
                0.00    0.00       1/1           global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4331]
                0.00    0.00       1/1           global constructors keyed to feOptSpec [4324]
                0.00    0.00       1/1           global constructors keyed to sip_command_bin [4339]
                0.00    0.00       1/1           global constructors keyed to iiCurrArgs [4329]
                0.00    0.00       1/1           global constructors keyed to slists_bin [4340]
                0.00    0.00       1/1           global constructors keyed to rnumber_bin [4335]
                0.00    0.00       1/1           global constructors keyed to ip_smatrix_bin [4330]
                0.00    0.00       1/1           global constructors keyed to MP_INT_bin [4317]
                0.00    0.00       1/1           global constructors keyed to gmp_output_digits [4328]
                0.00    0.00       1/1           global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4341]
                0.00    0.00       1/1           global constructors keyed to fglmUpdatesource(sip_sideal *) [4326]
                0.00    0.00       1/1           global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4321]
                0.00    0.00       1/1           global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4319]
                0.00    0.00       1/1           global constructors keyed to fglmcomb.cc [4327]
                0.00    0.00       1/1           global constructors keyed to s_si_link_extension_bin [4337]
                0.00    0.00       1/1           global constructors keyed to sSubexpr_bin [4336]
                0.00    0.00       1/1           global constructors keyed to mpsr_sleftv_bin [4333]
                0.00    0.00       1/1           global constructors keyed to kBucketCreate(sip_sring *) [4332]
                0.00    0.00       1/1           global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4320]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [100]
[106]    0.0    0.00    0.00      82         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [106]
                0.00    0.00      82/167         __builtin_vec_new [99]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [100]
[107]    0.0    0.00    0.00      82         Array<CanonicalForm>::Array(int) [107]
                0.00    0.00      82/167         __builtin_vec_new [99]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[108]    0.0    0.00    0.00       7         jjPLUS_P(sleftv *, sleftv *, sleftv *) [108]
                0.00    0.00       7/107354      p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [13]
                0.00    0.00      14/133         sleftv::CopyD(int) [80]
                0.00    0.00       7/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [166]
-----------------------------------------------
                0.00    0.00      18/18          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [87]
[109]    0.0    0.00    0.00      18         chainCrit(spolyrec *, int, skStrategy *) [109]
                0.00    0.00      50/9263        enterL(sLObject **, int *, int *, sLObject, int) [51]
                0.00    0.00      50/234081      posInL17(sLObject *, int, sLObject *, skStrategy *) [20]
                0.00    0.00     106/106         pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [167]
                0.00    0.00      21/78          deleteInL(sLObject *, int *, int, skStrategy *) [175]
-----------------------------------------------
                0.00    0.00       2/74          iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00      11/74          iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      23/74          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      38/74          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[110]    0.0    0.00    0.00      74         iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00      20/133         sleftv::CopyD(int) [80]
                0.00    0.00      20/1176        sleftv::Typ(void) [140]
                0.00    0.00      18/18          iiS2Link(void *) [210]
                0.00    0.00       1/1           iiDummy(void *) [300]
                0.00    0.00       1/1           iiI2P(void *) [302]
-----------------------------------------------
                0.00    0.00     107/107         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
[111]    0.0    0.00    0.00     107         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.00    0.00     214/415907      pDeg(spolyrec *, sip_sring *) [26]
-----------------------------------------------
                                                 <spontaneous>
[112]    0.0    0.00    0.00                 opendir [112]
                0.00    0.00       3/36          malloc [86]
                0.00    0.00       3/3           calloc [271]
-----------------------------------------------
                0.00    0.00       2/13          newFile(char *, _IO_FILE *) [127]
                0.00    0.00       2/13          iiLibCmd(char *, short) [90]
                0.00    0.00       4/13          main [1]
                0.00    0.00       5/13          iiGetLibProcBuffer(procinfo *, int) [115]
[113]    0.0    0.00    0.00      13         feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00       5/32070       omAllocBinFromFullPage [23]
                0.00    0.00       5/34297       omFreeToPageFault [35]
                0.00    0.00       8/8           feResource(char, int) [240]
-----------------------------------------------
                0.00    0.00     107/107         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
[114]    0.0    0.00    0.00     107         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
                0.00    0.00     117/747137      nDummy1(snumber **) [24]
                0.00    0.00     214/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
                0.00    0.00      10/21          npMult(snumber *, snumber *) [204]
                0.00    0.00       5/5           npEqual(snumber *, snumber *) [256]
-----------------------------------------------
                0.00    0.00       5/5           iiPStart(idrec *, sleftv *) <cycle 1> [71]
[115]    0.0    0.00    0.00       5         iiGetLibProcBuffer(procinfo *, int) [115]
                0.00    0.00       5/13          feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00       2/32070       omAllocBinFromFullPage [23]
                0.00    0.00      10/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [199]
                0.00    0.00       5/5           iiProcArgs(char *, short) [255]
-----------------------------------------------
                                                 <spontaneous>
[116]    0.0    0.00    0.00                 __gmpf_init_set_d [116]
                0.00    0.00       2/36          malloc [86]
-----------------------------------------------
                                                 <spontaneous>
[117]    0.0    0.00    0.00                 __gmpz_init_set_si [117]
                0.00    0.00       2/36          malloc [86]
-----------------------------------------------
                0.00    0.00      29/29          ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
[118]    0.0    0.00    0.00      29         ksCheckCoeff(snumber **, snumber **) [118]
                0.00    0.00      87/747000      npIsOne(snumber *) [29]
                0.00    0.00      29/747137      nDummy1(snumber **) [24]
                0.00    0.00      58/58          ndCopy(snumber *) [182]
                0.00    0.00      29/29          ndGcd(snumber *, snumber *) [198]
-----------------------------------------------
                                  88             yyparse(void) <cycle 1> [33]
[119]    0.0    0.00    0.00      88         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      88/346         sleftv::listLength(void) [154]
                0.00    0.00      88/988         sleftv::CleanUp(void) [141]
                0.00    0.00      33/33          jjSTRING_PL(sleftv *, sleftv *) [189]
                0.00    0.00      25/25          jjSYSTEM(sleftv *, sleftv *) [202]
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [312]
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [213]
-----------------------------------------------
                0.00    0.00      11/11          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
[120]    0.0    0.00    0.00      11         iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      11/74          iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00      22/1176        sleftv::Typ(void) [140]
                0.00    0.00      11/79          iiTestConvert(int, int) [174]
                0.00    0.00      11/575         sleftv::Data(void) [147]
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [236]
                0.00    0.00      11/988         sleftv::CleanUp(void) [141]
-----------------------------------------------
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [231]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [214]
[121]    0.0    0.00    0.00      32         iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
                0.00    0.00      32/32          iiCheckNest(void) [125]
                0.00    0.00      32/33          namerec::push(sip_package *, char *, int, short) [124]
                0.00    0.00      32/32          iiConvName(char *) [192]
                0.00    0.00      32/32          namerec::pop(short) [196]
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [71]
-----------------------------------------------
                0.00    0.00       1/122         yyparse(void) <cycle 1> [33]
                0.00    0.00       1/122         iiLibCmd(char *, short) [90]
                0.00    0.00      41/122         yylplex(char *, char *, lib_style_types *, lp_modes) [92]
                0.00    0.00      79/122         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [130]
[122]    0.0    0.00    0.00     122         enterid(char *, int, int, idrec **, short) [122]
                0.00    0.00     122/122         idrec::set(char *, int, int, short) [123]
-----------------------------------------------
                0.00    0.00     122/122         enterid(char *, int, int, idrec **, short) [122]
[123]    0.0    0.00    0.00     122         idrec::set(char *, int, int, short) [123]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
                0.00    0.00       2/6           idInit(int, int) [252]
-----------------------------------------------
                0.00    0.00       1/33          main [1]
                0.00    0.00      32/33          iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
[124]    0.0    0.00    0.00      33         namerec::push(sip_package *, char *, int, short) [124]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
[125]    0.0    0.00    0.00      32         iiCheckNest(void) [125]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[126]    0.0    0.00    0.00       1         slStandardInit(void) [126]
                0.00    0.00       1/32070       omAllocBinFromFullPage [23]
                0.00    0.00       1/1           slInitDBMExtension(s_si_link_extension *) [346]
                0.00    0.00       1/1           slInitMPFileExtension(s_si_link_extension *) [347]
                0.00    0.00       1/1           slInitMPTcpExtension(s_si_link_extension *) [348]
-----------------------------------------------
                0.00    0.00       2/2           main [1]
[127]    0.0    0.00    0.00       2         newFile(char *, _IO_FILE *) [127]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [113]
                0.00    0.00       2/64          __builtin_new [4309]
-----------------------------------------------
                0.00    0.00      26/26          yyparse(void) <cycle 1> [33]
[128]    0.0    0.00    0.00      26         sleftv::Print(sleftv *, int) [128]
                0.00    0.00       1/133         sleftv::CopyD(int) [80]
                0.00    0.00      27/1176        sleftv::Typ(void) [140]
                0.00    0.00      26/575         sleftv::Data(void) [147]
                0.00    0.00      26/988         sleftv::CleanUp(void) [141]
                0.00    0.00       1/85          Print [172]
                0.00    0.00       1/1           PrintLn [288]
-----------------------------------------------
                0.00    0.00       1/1           jiAssign_1(sleftv *, sleftv *) [78]
[129]    0.0    0.00    0.00       1         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [129]
                0.00    0.00       1/133         sleftv::CopyD(int) [80]
                0.00    0.00       1/1           pNormalize(spolyrec *) [326]
                0.00    0.00       1/94          sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00      79/79          yyparse(void) <cycle 1> [33]
[130]    0.0    0.00    0.00      79         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [130]
                0.00    0.00      79/122         enterid(char *, int, int, idrec **, short) [122]
                0.00    0.00      79/988         sleftv::CleanUp(void) [141]
-----------------------------------------------
                0.00    0.00       4/4           jjJACOB_P(sleftv *, sleftv *) [132]
[131]    0.0    0.00    0.00       4         pDiff(spolyrec *, int) [131]
                0.00    0.00      11/747137      nDummy1(snumber **) [24]
                0.00    0.00      11/875         npInit(int) [142]
                0.00    0.00      11/21          npMult(snumber *, snumber *) [204]
                0.00    0.00      11/26          npIsZero(snumber *) [200]
                0.00    0.00      11/443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[132]    0.0    0.00    0.00       1         jjJACOB_P(sleftv *, sleftv *) [132]
                0.00    0.00       4/4           pDiff(spolyrec *, int) [131]
                0.00    0.00       1/6           idInit(int, int) [252]
                0.00    0.00       1/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       4/4           initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
[133]    0.0    0.00    0.00       4         initEcartNormal(sLObject *) [133]
                0.00    0.00       4/415907      pDeg(spolyrec *, sip_sring *) [26]
                0.00    0.00       4/4           pLDeg0c(spolyrec *, int *, sip_sring *) [137]
-----------------------------------------------
                0.00    0.00       2/2           idHomIdeal(sip_sideal *, sip_sideal *) [135]
[134]    0.0    0.00    0.00       2         pIsHomogeneous(spolyrec *) [134]
                0.00    0.00       5/415907      pDeg(spolyrec *, sip_sring *) [26]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[135]    0.0    0.00    0.00       1         idHomIdeal(sip_sideal *, sip_sideal *) [135]
                0.00    0.00       2/2           pIsHomogeneous(spolyrec *) [134]
-----------------------------------------------
                                                 <spontaneous>
[136]    0.0    0.00    0.00                 global constructors keyed to ff_prime [136]
                0.00    0.00       1/167         __builtin_vec_new [99]
-----------------------------------------------
                0.00    0.00       4/4           initEcartNormal(sLObject *) [133]
[137]    0.0    0.00    0.00       4         pLDeg0c(spolyrec *, int *, sip_sring *) [137]
                0.00    0.00       4/415907      pDeg(spolyrec *, sip_sring *) [26]
-----------------------------------------------
[138]    0.0    0.00    0.00      12+18      <cycle 2 as a whole> [138]
                0.00    0.00      20             feResource(feResourceConfig_s *, int) <cycle 2> [206]
                0.00    0.00       6             feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
-----------------------------------------------
                0.00    0.00    2262/2262        omFreeToPageFault [35]
[139]    0.0    0.00    0.00    2262         omFreeBinPages [139]
                0.00    0.00      16/16          omFreeBinPagesRegion [224]
-----------------------------------------------
                                   4             sleftv::Typ(void) [140]
                0.00    0.00       1/1176        setOption(sleftv *, sleftv *) [345]
                0.00    0.00       1/1176        rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00       2/1176        exprlist_length(sleftv *) [291]
                0.00    0.00      12/1176        jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      20/1176        iiConvert(int, int, int, sleftv *, sleftv *) [110]
                0.00    0.00      21/1176        iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      22/1176        iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      27/1176        sleftv::Print(sleftv *, int) [128]
                0.00    0.00      32/1176        sleftv::Copy(sleftv *) [190]
                0.00    0.00      43/1176        jjSYSTEM(sleftv *, sleftv *) [202]
                0.00    0.00      44/1176        sleftv::String(void *, short, int) [184]
                0.00    0.00      64/1176        killlocals(int) [194]
                0.00    0.00      67/1176        iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      76/1176        yyparse(void) <cycle 1> [33]
                0.00    0.00     186/1176        jiAssign_1(sleftv *, sleftv *) [78]
                0.00    0.00     214/1176        iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00     344/1176        iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[140]    0.0    0.00    0.00    1176+4       sleftv::Typ(void) [140]
                                   4             sleftv::Typ(void) [140]
-----------------------------------------------
                                  49             sleftv::CleanUp(void) [141]
                0.00    0.00       1/988         iiExport(sleftv *, int) [301]
                0.00    0.00       2/988         rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00      11/988         iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      12/988         jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      12/988         killhdl(idrec *, idrec **) [176]
                0.00    0.00      21/988         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      26/988         sleftv::Print(sleftv *, int) [128]
                0.00    0.00      63/988         yyparse(void) <cycle 1> [33]
                0.00    0.00      79/988         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [130]
                0.00    0.00      88/988         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
                0.00    0.00      90/988         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     201/988         iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00     382/988         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[141]    0.0    0.00    0.00     988+49      sleftv::CleanUp(void) [141]
                0.00    0.00      18/18          slKill(sip_link *) [220]
                0.00    0.00       1/63          __builtin_delete [4310]
                                  49             sleftv::CleanUp(void) [141]
-----------------------------------------------
                0.00    0.00       1/875         nInitChar(sip_sring *) [48]
                0.00    0.00      11/875         pDiff(spolyrec *, int) [131]
                0.00    0.00      15/875         p_ISet(int, sip_sring *) [227]
                0.00    0.00      29/875         ndGcd(snumber *, snumber *) [198]
                0.00    0.00     819/875         pNorm(spolyrec *) [16]
[142]    0.0    0.00    0.00     875         npInit(int) [142]
-----------------------------------------------
                0.00    0.00      15/842         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [10]
                0.00    0.00      18/842         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00     809/842         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[143]    0.0    0.00    0.00     842         kBucketDestroy(kBucket **) [143]
-----------------------------------------------
                0.00    0.00       4/842         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      29/842         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00     809/842         doRed(sLObject *, sTObject *, short, skStrategy *) [9]
[144]    0.0    0.00    0.00     842         kBucketInit(kBucket *, spolyrec *, int) [144]
                0.00    0.00       1/5           pLength(spolyrec *) [257]
-----------------------------------------------
                0.00    0.00     827/827         enterT(sLObject, skStrategy *, int) [21]
[145]    0.0    0.00    0.00     827         posInT17(sTObject *, int, sLObject const &) [145]
-----------------------------------------------
                0.00    0.00     774/774         yylex(MYYSTYPE *) [43]
[146]    0.0    0.00    0.00     774         yy_get_previous_state(void) [146]
-----------------------------------------------
                                   4             sleftv::Data(void) [147]
                0.00    0.00       1/575         jjSTD(sleftv *, sleftv *) [5]
                0.00    0.00       1/575         jjJACOB_P(sleftv *, sleftv *) [132]
                0.00    0.00       1/575         iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00       1/575         exprlist_length(sleftv *) [291]
                0.00    0.00       1/575         rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00       3/575         jjMEMORY(sleftv *, sleftv *) [272]
                0.00    0.00       4/575         jjMakeSub(sleftv *) [263]
                0.00    0.00       7/575         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00       7/575         jjREAD(sleftv *, sleftv *) [244]
                0.00    0.00      10/575         sleftv::CopyD(int) [80]
                0.00    0.00      11/575         iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      12/575         jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      14/575         jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
                0.00    0.00      18/575         jjCOUNT_L(sleftv *, sleftv *) [211]
                0.00    0.00      21/575         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [243]
                0.00    0.00      25/575         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [201]
                0.00    0.00      26/575         sleftv::Print(sleftv *, int) [128]
                0.00    0.00      28/575         jjMINUS_I(sleftv *, sleftv *, sleftv *) [229]
                0.00    0.00      32/575         sleftv::Copy(sleftv *) [190]
                0.00    0.00      36/575         jjGT_I(sleftv *, sleftv *, sleftv *) [212]
                0.00    0.00      41/575         jjSYSTEM(sleftv *, sleftv *) [202]
                0.00    0.00      44/575         sleftv::String(void *, short, int) [184]
                0.00    0.00      51/575         yyparse(void) <cycle 1> [33]
                0.00    0.00     180/575         jjPLUS_S(sleftv *, sleftv *, sleftv *) [170]
[147]    0.0    0.00    0.00     575+4       sleftv::Data(void) [147]
                0.00    0.00     437/560         iiCheckRing(int) [148]
                0.00    0.00       2/2           getTimer(void) [282]
                                   4             sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00     123/560         sleftv::CopyD(int) [80]
                0.00    0.00     437/560         sleftv::Data(void) [147]
[148]    0.0    0.00    0.00     560         iiCheckRing(int) [148]
-----------------------------------------------
                0.00    0.00     512/512         yylex(MYYSTYPE *) [43]
[149]    0.0    0.00    0.00     512         IsCmd(char *, int &) [149]
-----------------------------------------------
                0.00    0.00       5/443         pPower(spolyrec *, int) [75]
                0.00    0.00      11/443         pDiff(spolyrec *, int) [131]
                0.00    0.00      14/443         syMake(sleftv *, char *, idrec *) [62]
                0.00    0.00      58/443         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [67]
                0.00    0.00     141/443         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
                0.00    0.00     214/443         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
[150]    0.0    0.00    0.00     443         p_Setm_TotalDegree(spolyrec *, sip_sring *) [150]
-----------------------------------------------
                0.00    0.00     403/403         yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[151]    0.0    0.00    0.00     403         current_pos(int) [151]
-----------------------------------------------
                0.00    0.00     379/379         feReadLine(char *, int) [93]
[152]    0.0    0.00    0.00     379         fePrintEcho(char *, char *) [152]
-----------------------------------------------
                0.00    0.00     347/347         yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[153]    0.0    0.00    0.00     347         copy_string(lp_modes) [153]
-----------------------------------------------
                0.00    0.00       1/346         rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00      12/346         jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      33/346         jjSTRING_PL(sleftv *, sleftv *) [189]
                0.00    0.00      88/346         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
                0.00    0.00     212/346         iiAssign(sleftv *, sleftv *) [73]
[154]    0.0    0.00    0.00     346         sleftv::listLength(void) [154]
-----------------------------------------------
                0.00    0.00       3/341         omSizeOfAddr [253]
                0.00    0.00     338/341         omSizeOfLargeAddr [156]
[155]    0.0    0.00    0.00     341         malloc_usable_size [155]
-----------------------------------------------
                0.00    0.00       1/338         exitVoice(void) [180]
                0.00    0.00       2/338         yy_flex_free(void *) [267]
                0.00    0.00       3/338         free [187]
                0.00    0.00       5/338         exitBuchMora(skStrategy *) [30]
                0.00    0.00      32/338         yy_flex_free(void *) [179]
                0.00    0.00      37/338         omDoRealloc [85]
                0.00    0.00     258/338         omRealloc0Large [171]
[156]    0.0    0.00    0.00     338         omSizeOfLargeAddr [156]
                0.00    0.00     338/341         malloc_usable_size [155]
-----------------------------------------------
                0.00    0.00     330/330         syMake(sleftv *, char *, idrec *) [62]
[157]    0.0    0.00    0.00     330         ggetid(char const *, short) [157]
-----------------------------------------------
                0.00    0.00      67/239         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     172/239         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[158]    0.0    0.00    0.00     239         iiTabIndex(sValCmdTab const *, int, int) [158]
-----------------------------------------------
                0.00    0.00     162/162         omGetUsedBinBytes [274]
[159]    0.0    0.00    0.00     162         omGetUsedBytesOfBin [159]
-----------------------------------------------
                0.00    0.00      59/149         omFreeSizeToSystem [181]
                0.00    0.00      90/149         rEALLOc [164]
[160]    0.0    0.00    0.00     149         fREe [160]
-----------------------------------------------
                0.00    0.00     141/141         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
[161]    0.0    0.00    0.00     141         pHasNotCF(spolyrec *, spolyrec *) [161]
-----------------------------------------------
                0.00    0.00     141/141         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
[162]    0.0    0.00    0.00     141         pLcm(spolyrec *, spolyrec *, spolyrec *) [162]
-----------------------------------------------
                0.00    0.00      16/139         omBinPageIndexFault [101]
                0.00    0.00      37/139         omDoRealloc [85]
                0.00    0.00      86/139         omRealloc0Large [171]
[163]    0.0    0.00    0.00     139         omReallocSizeFromSystem [163]
                0.00    0.00     139/139         rEALLOc [164]
-----------------------------------------------
                0.00    0.00     139/139         omReallocSizeFromSystem [163]
[164]    0.0    0.00    0.00     139         rEALLOc [164]
                0.00    0.00      90/149         fREe [160]
                0.00    0.00      13/82          mALLOc [173]
-----------------------------------------------
                0.00    0.00      23/125         syMake(sleftv *, char *, idrec *) [62]
                0.00    0.00     102/125         pmInit(char *, short &) [63]
[165]    0.0    0.00    0.00     125         rIsRingVar(char *) [165]
-----------------------------------------------
                0.00    0.00       7/111         jjPLUS_P(sleftv *, sleftv *, sleftv *) [108]
                0.00    0.00      14/111         jjMINUS_I(sleftv *, sleftv *, sleftv *) [229]
                0.00    0.00      90/111         jjPLUS_S(sleftv *, sleftv *, sleftv *) [170]
[166]    0.0    0.00    0.00     111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [166]
-----------------------------------------------
                0.00    0.00     106/106         chainCrit(spolyrec *, int, skStrategy *) [109]
[167]    0.0    0.00    0.00     106         pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [167]
-----------------------------------------------
                0.00    0.00       1/97          yylex(MYYSTYPE *) [43]
                0.00    0.00      32/97          yyrestart(_IO_FILE *) [197]
                0.00    0.00      32/97          myynewbuffer(void) [66]
                0.00    0.00      32/97          myyoldbuffer(void *) [195]
[168]    0.0    0.00    0.00      97         yy_load_buffer_state(void) [168]
-----------------------------------------------
                0.00    0.00       1/94          jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [129]
                0.00    0.00       1/94          iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00       2/94          jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [79]
                0.00    0.00      25/94          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [201]
                0.00    0.00      65/94          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [98]
[169]    0.0    0.00    0.00      94         sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00      90/90          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[170]    0.0    0.00    0.00      90         jjPLUS_S(sleftv *, sleftv *, sleftv *) [170]
                0.00    0.00     180/575         sleftv::Data(void) [147]
                0.00    0.00      90/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [166]
-----------------------------------------------
                0.00    0.00      86/86          omDoRealloc [85]
[171]    0.0    0.00    0.00      86         omRealloc0Large [171]
                0.00    0.00     258/338         omSizeOfLargeAddr [156]
                0.00    0.00      86/139         omReallocSizeFromSystem [163]
-----------------------------------------------
                0.00    0.00       1/85          main [1]
                0.00    0.00       1/85          iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [91]
                0.00    0.00       1/85          messageStat(int, int, int, skStrategy *) [315]
                0.00    0.00       1/85          sleftv::Print(sleftv *, int) [128]
                0.00    0.00      81/85          message(int, int *, int *, skStrategy *) [42]
[172]    0.0    0.00    0.00      85         Print [172]
-----------------------------------------------
                0.00    0.00      13/82          rEALLOc [164]
                0.00    0.00      69/82          omAllocFromSystem [56]
[173]    0.0    0.00    0.00      82         mALLOc [173]
                0.00    0.00      11/11          malloc_extend_top [233]
-----------------------------------------------
                0.00    0.00       3/79          iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00      11/79          iiWRITE(sleftv *, sleftv *) [120]
                0.00    0.00      23/79          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      42/79          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[174]    0.0    0.00    0.00      79         iiTestConvert(int, int) [174]
-----------------------------------------------
                0.00    0.00      21/78          chainCrit(spolyrec *, int, skStrategy *) [109]
                0.00    0.00      57/78          enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [89]
[175]    0.0    0.00    0.00      78         deleteInL(sLObject *, int *, int, skStrategy *) [175]
-----------------------------------------------
                0.00    0.00      69/69          killlocals(int) [194]
[176]    0.0    0.00    0.00      69         killhdl(idrec *, idrec **) [176]
                0.00    0.00      12/988         sleftv::CleanUp(void) [141]
-----------------------------------------------
                0.00    0.00      65/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [178]
[177]    0.0    0.00    0.00      65         yy_flush_buffer(yy_buffer_state *) [177]
-----------------------------------------------
                0.00    0.00       1/65          yy_create_buffer(_IO_FILE *, int) [103]
                0.00    0.00      32/65          yyrestart(_IO_FILE *) [197]
                0.00    0.00      32/65          myynewbuffer(void) [66]
[178]    0.0    0.00    0.00      65         yy_init_buffer(yy_buffer_state *, _IO_FILE *) [178]
                0.00    0.00      65/65          yy_flush_buffer(yy_buffer_state *) [177]
-----------------------------------------------
                0.00    0.00      64/64          myyoldbuffer(void *) [195]
[179]    0.0    0.00    0.00      64         yy_flex_free(void *) [179]
                0.00    0.00      32/338         omSizeOfLargeAddr [156]
                0.00    0.00      32/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.00      16/61          yylex(MYYSTYPE *) [43]
                0.00    0.00      45/61          exitBuffer(feBufferTypes) [191]
[180]    0.0    0.00    0.00      61         exitVoice(void) [180]
                0.00    0.00      61/63          __builtin_delete [4310]
                0.00    0.00      32/32          myyoldbuffer(void *) [195]
                0.00    0.00       1/338         omSizeOfLargeAddr [156]
                0.00    0.00       1/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.00       1/59          exitVoice(void) [180]
                0.00    0.00       2/59          yy_flex_free(void *) [267]
                0.00    0.00       3/59          free [187]
                0.00    0.00       5/59          exitBuchMora(skStrategy *) [30]
                0.00    0.00      16/59          omFreeBinPagesRegion [224]
                0.00    0.00      32/59          yy_flex_free(void *) [179]
[181]    0.0    0.00    0.00      59         omFreeSizeToSystem [181]
                0.00    0.00      59/149         fREe [160]
-----------------------------------------------
                0.00    0.00      58/58          ksCheckCoeff(snumber **, snumber **) [118]
[182]    0.0    0.00    0.00      58         ndCopy(snumber *) [182]
-----------------------------------------------
                0.00    0.00      49/49          pmInit(char *, short &) [63]
[183]    0.0    0.00    0.00      49         npRead(char *, snumber **) [183]
-----------------------------------------------
                0.00    0.00      11/44          slWriteAscii(sip_link *, sleftv *) [235]
                0.00    0.00      33/44          jjSTRING_PL(sleftv *, sleftv *) [189]
[184]    0.0    0.00    0.00      44         sleftv::String(void *, short, int) [184]
                0.00    0.00      44/575         sleftv::Data(void) [147]
                0.00    0.00      44/1176        sleftv::Typ(void) [140]
-----------------------------------------------
                0.00    0.00      44/44          cancelunit(sLObject *) [203]
[185]    0.0    0.00    0.00      44         rIsPolyVar(int, sip_sring *) [185]
-----------------------------------------------
                0.00    0.00      41/41          yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[186]    0.0    0.00    0.00      41         iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [186]
-----------------------------------------------
                0.00    0.00       6/34          closedir [983]
                0.00    0.00      28/34          fclose [1217]
[187]    0.0    0.00    0.00      34         free [187]
                0.00    0.00       3/338         omSizeOfLargeAddr [156]
                0.00    0.00       3/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.00       4/33          feCleanResourceValue(feResourceType, char *) [249]
                0.00    0.00      29/33          feCleanUpPath(char *) [277]
[188]    0.0    0.00    0.00      33         feCleanUpFile(char *) [188]
                0.00    0.00       4/4           mystrcpy(char *, char *) [265]
-----------------------------------------------
                0.00    0.00      33/33          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
[189]    0.0    0.00    0.00      33         jjSTRING_PL(sleftv *, sleftv *) [189]
                0.00    0.00      33/346         sleftv::listLength(void) [154]
                0.00    0.00      33/44          sleftv::String(void *, short, int) [184]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      20/32          yyparse(void) <cycle 1> [33]
[190]    0.0    0.00    0.00      32         sleftv::Copy(sleftv *) [190]
                0.00    0.00      32/1176        sleftv::Typ(void) [140]
                0.00    0.00      32/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00      32/32          yyparse(void) <cycle 1> [33]
[191]    0.0    0.00    0.00      32         exitBuffer(feBufferTypes) [191]
                0.00    0.00      45/61          exitVoice(void) [180]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
[192]    0.0    0.00    0.00      32         iiConvName(char *) [192]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      20/32          iiAssign(sleftv *, sleftv *) [73]
[193]    0.0    0.00    0.00      32         ipMoveId(idrec *) [193]
                0.00    0.00       9/9           lRingDependend(slists *) [237]
-----------------------------------------------
                0.00    0.00      32/32          iiPStart(idrec *, sleftv *) <cycle 1> [71]
[194]    0.0    0.00    0.00      32         killlocals(int) [194]
                0.00    0.00      69/69          killhdl(idrec *, idrec **) [176]
                0.00    0.00      64/1176        sleftv::Typ(void) [140]
-----------------------------------------------
                0.00    0.00      32/32          exitVoice(void) [180]
[195]    0.0    0.00    0.00      32         myyoldbuffer(void *) [195]
                0.00    0.00      64/64          yy_flex_free(void *) [179]
                0.00    0.00      32/97          yy_load_buffer_state(void) [168]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
[196]    0.0    0.00    0.00      32         namerec::pop(short) [196]
-----------------------------------------------
                0.00    0.00      16/32          yylex(MYYSTYPE *) [43]
                0.00    0.00      16/32          yy_get_next_buffer(void) [94]
[197]    0.0    0.00    0.00      32         yyrestart(_IO_FILE *) [197]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [178]
                0.00    0.00      32/97          yy_load_buffer_state(void) [168]
-----------------------------------------------
                0.00    0.00      29/29          ksCheckCoeff(snumber **, snumber **) [118]
[198]    0.0    0.00    0.00      29         ndGcd(snumber *, snumber *) [198]
                0.00    0.00      29/875         npInit(int) [142]
-----------------------------------------------
                0.00    0.00       7/26          slReadAscii2(sip_link *, sleftv *) [245]
                0.00    0.00       9/26          libread(_IO_FILE *, char *, int) [238]
                0.00    0.00      10/26          iiGetLibProcBuffer(procinfo *, int) [115]
[199]    0.0    0.00    0.00      26         myfread(void *, unsigned int, unsigned int, _IO_FILE *) [199]
-----------------------------------------------
                0.00    0.00      11/26          pDiff(spolyrec *, int) [131]
                0.00    0.00      15/26          p_ISet(int, sip_sring *) [227]
[200]    0.0    0.00    0.00      26         npIsZero(snumber *) [200]
-----------------------------------------------
                0.00    0.00      25/25          jiAssign_1(sleftv *, sleftv *) [78]
[201]    0.0    0.00    0.00      25         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [201]
                0.00    0.00      25/575         sleftv::Data(void) [147]
                0.00    0.00      25/94          sleftv::LData(void) [169]
-----------------------------------------------
                0.00    0.00      25/25          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
[202]    0.0    0.00    0.00      25         jjSYSTEM(sleftv *, sleftv *) [202]
                0.00    0.00      43/1176        sleftv::Typ(void) [140]
                0.00    0.00      41/575         sleftv::Data(void) [147]
                0.00    0.00       2/2           feGetOptIndex(char const *) [278]
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [295]
-----------------------------------------------
                0.00    0.00       4/22          initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00      18/22          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[203]    0.0    0.00    0.00      22         cancelunit(sLObject *) [203]
                0.00    0.00      44/44          rIsPolyVar(int, sip_sring *) [185]
-----------------------------------------------
                0.00    0.00      10/21          ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
                0.00    0.00      11/21          pDiff(spolyrec *, int) [131]
[204]    0.0    0.00    0.00      21         npMult(snumber *, snumber *) [204]
-----------------------------------------------
                0.00    0.00       1/20          feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
                0.00    0.00       4/20          feInitResources(char *) [293]
                0.00    0.00       7/20          feSprintf(char *, char const *, int) <cycle 2> [260]
                0.00    0.00       8/20          feResource(char, int) [240]
[205]    0.0    0.00    0.00      20         feGetResourceConfig(char) [205]
-----------------------------------------------
                                   1             feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
                                   7             feSprintf(char *, char const *, int) <cycle 2> [260]
                0.00    0.00       4/12          feInitResources(char *) [293]
                0.00    0.00       8/12          feResource(char, int) [240]
[206]    0.0    0.00    0.00      20         feResource(feResourceConfig_s *, int) <cycle 2> [206]
                                   6             feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
-----------------------------------------------
                0.00    0.00      18/18          enterSMora(sLObject, int, skStrategy *, int) [209]
[207]    0.0    0.00    0.00      18         HEckeTest(spolyrec *, skStrategy *) [207]
                0.00    0.00      18/18          p_IsPurePower(spolyrec *, sip_sring *) [215]
-----------------------------------------------
                0.00    0.00      18/18          enterSMora(sLObject, int, skStrategy *, int) [209]
[208]    0.0    0.00    0.00      18         enterSBba(sLObject, int, skStrategy *, int) [208]
-----------------------------------------------
                0.00    0.00      18/18          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[209]    0.0    0.00    0.00      18         enterSMora(sLObject, int, skStrategy *, int) [209]
                0.00    0.00      18/18          enterSBba(sLObject, int, skStrategy *, int) [208]
                0.00    0.00      18/18          HEckeTest(spolyrec *, skStrategy *) [207]
-----------------------------------------------
                0.00    0.00      18/18          iiConvert(int, int, int, sleftv *, sleftv *) [110]
[210]    0.0    0.00    0.00      18         iiS2Link(void *) [210]
                0.00    0.00      18/18          slInit(sip_link *, char *) [219]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[211]    0.0    0.00    0.00      18         jjCOUNT_L(sleftv *, sleftv *) [211]
                0.00    0.00      18/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[212]    0.0    0.00    0.00      18         jjGT_I(sleftv *, sleftv *, sleftv *) [212]
                0.00    0.00      36/575         sleftv::Data(void) [147]
-----------------------------------------------
                                  18             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
[213]    0.0    0.00    0.00      18         jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [213]
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
-----------------------------------------------
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[214]    0.0    0.00    0.00      18         jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [214]
                                  18             iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
-----------------------------------------------
                0.00    0.00      18/18          HEckeTest(spolyrec *, skStrategy *) [207]
[215]    0.0    0.00    0.00      18         p_IsPurePower(spolyrec *, sip_sring *) [215]
-----------------------------------------------
                0.00    0.00      18/18          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[216]    0.0    0.00    0.00      18         redtail(spolyrec *, int, skStrategy *) [216]
-----------------------------------------------
                0.00    0.00      18/18          slKill(sip_link *) [220]
[217]    0.0    0.00    0.00      18         slCleanUp(sip_link *) [217]
                0.00    0.00      18/18          slCloseAscii(sip_link *) [218]
-----------------------------------------------
                0.00    0.00      18/18          slCleanUp(sip_link *) [217]
[218]    0.0    0.00    0.00      18         slCloseAscii(sip_link *) [218]
-----------------------------------------------
                0.00    0.00      18/18          iiS2Link(void *) [210]
[219]    0.0    0.00    0.00      18         slInit(sip_link *, char *) [219]
-----------------------------------------------
                0.00    0.00      18/18          sleftv::CleanUp(void) [141]
[220]    0.0    0.00    0.00      18         slKill(sip_link *) [220]
                0.00    0.00      18/18          slCleanUp(sip_link *) [217]
-----------------------------------------------
                0.00    0.00       7/18          slRead(sip_link *, sleftv *) [247]
                0.00    0.00      11/18          slWrite(sip_link *, sleftv *) [236]
[221]    0.0    0.00    0.00      18         slOpenAscii(sip_link *, short) [221]
-----------------------------------------------
                0.00    0.00      17/17          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[222]    0.0    0.00    0.00      17         posInS(spolyrec **, int, spolyrec *) [222]
-----------------------------------------------
                0.00    0.00      16/16          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[223]    0.0    0.00    0.00      16         jjDEFINED(sleftv *, sleftv *) [223]
-----------------------------------------------
                0.00    0.00      16/16          omFreeBinPages [139]
[224]    0.0    0.00    0.00      16         omFreeBinPagesRegion [224]
                0.00    0.00      16/16          omUnregisterBinPages [225]
                0.00    0.00      16/16          omVfreeToSystem [226]
                0.00    0.00      16/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.00      16/16          omFreeBinPagesRegion [224]
[225]    0.0    0.00    0.00      16         omUnregisterBinPages [225]
-----------------------------------------------
                0.00    0.00      16/16          omFreeBinPagesRegion [224]
[226]    0.0    0.00    0.00      16         omVfreeToSystem [226]
-----------------------------------------------
                0.00    0.00       1/16          iiI2P(void *) [302]
                0.00    0.00       1/16          pPower(spolyrec *, int) [75]
                0.00    0.00      14/16          syMake(sleftv *, char *, idrec *) [62]
[227]    0.0    0.00    0.00      16         p_ISet(int, sip_sring *) [227]
                0.00    0.00      15/875         npInit(int) [142]
                0.00    0.00      15/26          npIsZero(snumber *) [200]
-----------------------------------------------
                0.00    0.00      14/14          SetProcs(p_Field, p_Length, p_Ord) [289]
[228]    0.0    0.00    0.00      14         FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [228]
-----------------------------------------------
                0.00    0.00      14/14          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[229]    0.0    0.00    0.00      14         jjMINUS_I(sleftv *, sleftv *, sleftv *) [229]
                0.00    0.00      28/575         sleftv::Data(void) [147]
                0.00    0.00      14/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [166]
-----------------------------------------------
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [74]
[230]    0.0    0.00    0.00      14         jjOP_REST(sleftv *, sleftv *, sleftv *) [230]
-----------------------------------------------
                                  14             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[231]    0.0    0.00    0.00      14         jjPROC1(sleftv *, sleftv *) <cycle 1> [231]
                                  14             iiMake_proc(idrec *, sleftv *) <cycle 1> [121]
-----------------------------------------------
                0.00    0.00      12/12          iiAssign(sleftv *, sleftv *) [73]
[232]    0.0    0.00    0.00      12         jjA_L_LIST(sleftv *, sleftv *) [232]
                0.00    0.00      12/346         sleftv::listLength(void) [154]
                0.00    0.00      12/1176        sleftv::Typ(void) [140]
                0.00    0.00      12/32          sleftv::Copy(sleftv *) [190]
                0.00    0.00      12/575         sleftv::Data(void) [147]
                0.00    0.00      12/32          ipMoveId(idrec *) [193]
                0.00    0.00      12/988         sleftv::CleanUp(void) [141]
-----------------------------------------------
                0.00    0.00      11/11          mALLOc [173]
[233]    0.0    0.00    0.00      11         malloc_extend_top [233]
-----------------------------------------------
                0.00    0.00       5/11          pPower(spolyrec *, int) [75]
                0.00    0.00       6/11          jjTIMES_P(sleftv *, sleftv *, sleftv *) [77]
[234]    0.0    0.00    0.00      11         p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [234]
-----------------------------------------------
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [236]
[235]    0.0    0.00    0.00      11         slWriteAscii(sip_link *, sleftv *) [235]
                0.00    0.00      11/44          sleftv::String(void *, short, int) [184]
-----------------------------------------------
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [120]
[236]    0.0    0.00    0.00      11         slWrite(sip_link *, sleftv *) [236]
                0.00    0.00      11/18          slOpenAscii(sip_link *, short) [221]
                0.00    0.00      11/11          slWriteAscii(sip_link *, sleftv *) [235]
-----------------------------------------------
                0.00    0.00       9/9           ipMoveId(idrec *) [193]
[237]    0.0    0.00    0.00       9         lRingDependend(slists *) [237]
-----------------------------------------------
                0.00    0.00       9/9           yy_get_next_buffer(void) [239]
[238]    0.0    0.00    0.00       9         libread(_IO_FILE *, char *, int) [238]
                0.00    0.00       9/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [199]
-----------------------------------------------
                0.00    0.00       9/9           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[239]    0.0    0.00    0.00       9         yy_get_next_buffer(void) [239]
                0.00    0.00       9/9           libread(_IO_FILE *, char *, int) [238]
                0.00    0.00       2/2           yylprestart(_IO_FILE *) [285]
-----------------------------------------------
                0.00    0.00       8/8           feFopen(char *, char *, char *, int, int) [113]
[240]    0.0    0.00    0.00       8         feResource(char, int) [240]
                0.00    0.00       8/20          feGetResourceConfig(char) [205]
                0.00    0.00       8/12          feResource(feResourceConfig_s *, int) <cycle 2> [206]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [247]
[241]    0.0    0.00    0.00       7         sleftv::Eval(void) [241]
-----------------------------------------------
                0.00    0.00       7/7           yyparse(void) <cycle 1> [33]
[242]    0.0    0.00    0.00       7         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      21/1176        sleftv::Typ(void) [140]
                0.00    0.00      21/988         sleftv::CleanUp(void) [141]
                0.00    0.00       7/7           jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [243]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [242]
[243]    0.0    0.00    0.00       7         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [243]
                0.00    0.00      21/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[244]    0.0    0.00    0.00       7         jjREAD(sleftv *, sleftv *) [244]
                0.00    0.00       7/575         sleftv::Data(void) [147]
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [247]
-----------------------------------------------
                0.00    0.00       7/7           slReadAscii(sip_link *) [246]
[245]    0.0    0.00    0.00       7         slReadAscii2(sip_link *, sleftv *) [245]
                0.00    0.00       7/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [199]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [247]
[246]    0.0    0.00    0.00       7         slReadAscii(sip_link *) [246]
                0.00    0.00       7/7           slReadAscii2(sip_link *, sleftv *) [245]
-----------------------------------------------
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [244]
[247]    0.0    0.00    0.00       7         slRead(sip_link *, sleftv *) [247]
                0.00    0.00       7/18          slOpenAscii(sip_link *, short) [221]
                0.00    0.00       7/7           slReadAscii(sip_link *) [246]
                0.00    0.00       7/7           sleftv::Eval(void) [241]
-----------------------------------------------
                0.00    0.00       7/7           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[248]    0.0    0.00    0.00       7         yy_get_previous_state(void) [248]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
[249]    0.0    0.00    0.00       6         feCleanResourceValue(feResourceType, char *) [249]
                0.00    0.00       4/33          feCleanUpFile(char *) [188]
                0.00    0.00       2/2           feCleanUpPath(char *) [277]
-----------------------------------------------
                                   6             feResource(feResourceConfig_s *, int) <cycle 2> [206]
[250]    0.0    0.00    0.00       6         feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
                0.00    0.00       6/6           feCleanResourceValue(feResourceType, char *) [249]
                0.00    0.00       6/6           feVerifyResourceValue(feResourceType, char *) [251]
                0.00    0.00       1/1           feGetExpandedExecutable(void) [292]
                0.00    0.00       1/20          feGetResourceConfig(char) [205]
                                   4             feSprintf(char *, char const *, int) <cycle 2> [260]
                                   1             feResource(feResourceConfig_s *, int) <cycle 2> [206]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
[251]    0.0    0.00    0.00       6         feVerifyResourceValue(feResourceType, char *) [251]
-----------------------------------------------
                0.00    0.00       1/6           jjJACOB_P(sleftv *, sleftv *) [132]
                0.00    0.00       1/6           idCopy(sip_sideal *) [84]
                0.00    0.00       1/6           iiAssign(sleftv *, sleftv *) [73]
                0.00    0.00       1/6           initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
                0.00    0.00       2/6           idrec::set(char *, int, int, short) [123]
[252]    0.0    0.00    0.00       6         idInit(int, int) [252]
-----------------------------------------------
                0.00    0.00       6/6           omDoRealloc [85]
[253]    0.0    0.00    0.00       6         omSizeOfAddr [253]
                0.00    0.00       3/341         malloc_usable_size [155]
-----------------------------------------------
                0.00    0.00       1/5           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       4/5           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[254]    0.0    0.00    0.00       5         sLObject::GetpLength(void) [254]
-----------------------------------------------
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [115]
[255]    0.0    0.00    0.00       5         iiProcArgs(char *, short) [255]
-----------------------------------------------
                0.00    0.00       5/5           ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [114]
[256]    0.0    0.00    0.00       5         npEqual(snumber *, snumber *) [256]
-----------------------------------------------
                0.00    0.00       1/5           kBucketInit(kBucket *, spolyrec *, int) [144]
                0.00    0.00       4/5           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[257]    0.0    0.00    0.00       5         pLength(spolyrec *) [257]
-----------------------------------------------
                0.00    0.00       4/4           initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
[258]    0.0    0.00    0.00       4         deleteHC(sLObject *, skStrategy *, short) [258]
-----------------------------------------------
                0.00    0.00       4/4           pmInit(char *, short &) [63]
[259]    0.0    0.00    0.00       4         eati(char *, int *) [259]
-----------------------------------------------
                                   4             feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
[260]    0.0    0.00    0.00       4         feSprintf(char *, char const *, int) <cycle 2> [260]
                0.00    0.00       7/20          feGetResourceConfig(char) [205]
                                   7             feResource(feResourceConfig_s *, int) <cycle 2> [206]
-----------------------------------------------
                0.00    0.00       2/4           iiLibCmd(char *, short) [90]
                0.00    0.00       2/4           iiExport(sleftv *, int) [301]
[261]    0.0    0.00    0.00       4         idrec::get(char const *, int) [261]
-----------------------------------------------
                0.00    0.00       4/4           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [72]
[262]    0.0    0.00    0.00       4         jjINDEX_I(sleftv *, sleftv *, sleftv *) [262]
                0.00    0.00       4/4           jjMakeSub(sleftv *) [263]
-----------------------------------------------
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [262]
[263]    0.0    0.00    0.00       4         jjMakeSub(sleftv *) [263]
                0.00    0.00       4/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       4/4           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[264]    0.0    0.00    0.00       4         make_version(char *, int) [264]
-----------------------------------------------
                0.00    0.00       4/4           feCleanUpFile(char *) [188]
[265]    0.0    0.00    0.00       4         mystrcpy(char *, char *) [265]
-----------------------------------------------
                0.00    0.00       4/4           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [298]
[266]    0.0    0.00    0.00       4         p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [266]
-----------------------------------------------
                0.00    0.00       4/4           reinit_yylp(void) [284]
[267]    0.0    0.00    0.00       4         yy_flex_free(void *) [267]
                0.00    0.00       2/338         omSizeOfLargeAddr [156]
                0.00    0.00       2/59          omFreeSizeToSystem [181]
-----------------------------------------------
                0.00    0.00       4/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [269]
[268]    0.0    0.00    0.00       4         yylp_flush_buffer(yy_buffer_state *) [268]
-----------------------------------------------
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [285]
                0.00    0.00       2/4           yylp_create_buffer(_IO_FILE *, int) [97]
[269]    0.0    0.00    0.00       4         yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [269]
                0.00    0.00       4/4           yylp_flush_buffer(yy_buffer_state *) [268]
-----------------------------------------------
                0.00    0.00       2/4           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [285]
[270]    0.0    0.00    0.00       4         yylp_load_buffer_state(void) [270]
-----------------------------------------------
                0.00    0.00       3/3           opendir [112]
[271]    0.0    0.00    0.00       3         calloc [271]
-----------------------------------------------
                0.00    0.00       3/3           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[272]    0.0    0.00    0.00       3         jjMEMORY(sleftv *, sleftv *) [272]
                0.00    0.00       3/3           omUpdateInfo [275]
                0.00    0.00       3/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       3/3           omUpdateInfo [275]
[273]    0.0    0.00    0.00       3         malloc_update_mallinfo [273]
-----------------------------------------------
                0.00    0.00       3/3           omUpdateInfo [275]
[274]    0.0    0.00    0.00       3         omGetUsedBinBytes [274]
                0.00    0.00     162/162         omGetUsedBytesOfBin [159]
-----------------------------------------------
                0.00    0.00       3/3           jjMEMORY(sleftv *, sleftv *) [272]
[275]    0.0    0.00    0.00       3         omUpdateInfo [275]
                0.00    0.00       3/3           malloc_update_mallinfo [273]
                0.00    0.00       3/3           omGetUsedBinBytes [274]
-----------------------------------------------
                0.00    0.00       1/3           rInit(sleftv *, sleftv *, sleftv *) [44]
                0.00    0.00       2/3           rSetHdl(idrec *, short) [50]
[276]    0.0    0.00    0.00       3         rDBTest(sip_sring *, char *, int) [276]
-----------------------------------------------
                0.00    0.00       2/2           feCleanResourceValue(feResourceType, char *) [249]
[277]    0.0    0.00    0.00       2         feCleanUpPath(char *) [277]
                0.00    0.00      29/33          feCleanUpFile(char *) [188]
-----------------------------------------------
                0.00    0.00       2/2           jjSYSTEM(sleftv *, sleftv *) [202]
[278]    0.0    0.00    0.00       2         feGetOptIndex(char const *) [278]
-----------------------------------------------
                0.00    0.00       2/2           fe_reset_input_mode(void) [280]
[279]    0.0    0.00    0.00       2         fe_reset_fe [279]
-----------------------------------------------
                0.00    0.00       2/2           m2_end [314]
[280]    0.0    0.00    0.00       2         fe_reset_input_mode(void) [280]
                0.00    0.00       2/2           fe_reset_fe [279]
-----------------------------------------------
                0.00    0.00       2/2           __gmpf_clear [4796]
[281]    0.0    0.00    0.00       2         freeSize [281]
-----------------------------------------------
                0.00    0.00       2/2           sleftv::Data(void) [147]
[282]    0.0    0.00    0.00       2         getTimer(void) [282]
-----------------------------------------------
                0.00    0.00       1/2           global constructors keyed to gmp_output_digits [4328]
                0.00    0.00       1/2           initCanonicalForm(void) [1730]
[283]    0.0    0.00    0.00       2         mmInit(void) [283]
                0.00    0.00       1/1           omInitInfo [321]
-----------------------------------------------
                0.00    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [91]
[284]    0.0    0.00    0.00       2         reinit_yylp(void) [284]
                0.00    0.00       4/4           yy_flex_free(void *) [267]
-----------------------------------------------
                0.00    0.00       2/2           yy_get_next_buffer(void) [239]
[285]    0.0    0.00    0.00       2         yylprestart(_IO_FILE *) [285]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [269]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [270]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [92]
[286]    0.0    0.00    0.00       2         yylpwrap [286]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [44]
[287]    0.0    0.00    0.00       1         IsPrime(int) [287]
-----------------------------------------------
                0.00    0.00       1/1           sleftv::Print(sleftv *, int) [128]
[288]    0.0    0.00    0.00       1         PrintLn [288]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [331]
[289]    0.0    0.00    0.00       1         SetProcs(p_Field, p_Length, p_Ord) [289]
                0.00    0.00      14/14          FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [228]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[290]    0.0    0.00    0.00       1         atGet(sleftv *, char *) [290]
-----------------------------------------------
                0.00    0.00       1/1           iiAssign(sleftv *, sleftv *) [73]
[291]    0.0    0.00    0.00       1         exprlist_length(sleftv *) [291]
                0.00    0.00       2/1176        sleftv::Typ(void) [140]
                0.00    0.00       1/575         sleftv::Data(void) [147]
-----------------------------------------------
                0.00    0.00       1/1           feInitResource(feResourceConfig_s *, int) <cycle 2> [250]
[292]    0.0    0.00    0.00       1         feGetExpandedExecutable(void) [292]
                0.00    0.00       1/1           omFindExec [317]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[293]    0.0    0.00    0.00       1         feInitResources(char *) [293]
                0.00    0.00       4/20          feGetResourceConfig(char) [205]
                0.00    0.00       4/12          feResource(feResourceConfig_s *, int) <cycle 2> [206]
-----------------------------------------------
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [295]
[294]    0.0    0.00    0.00       1         feOptAction(feOptIndex) [294]
-----------------------------------------------
                0.00    0.00       1/1           jjSYSTEM(sleftv *, sleftv *) [202]
[295]    0.0    0.00    0.00       1         feSetOptValue(feOptIndex, int) [295]
                0.00    0.00       1/1           feOptAction(feOptIndex) [294]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[296]    0.0    0.00    0.00       1         fe_getopt_long [296]
                0.00    0.00       1/1           _fe_getopt_internal [4343]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [317]
[297]    0.0    0.00    0.00       1         full_readlink [297]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[298]    0.0    0.00    0.00       1         idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [298]
                0.00    0.00       4/4           p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [266]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[299]    0.0    0.00    0.00       1         idSkipZeroes(sip_sideal *) [299]
-----------------------------------------------
                0.00    0.00       1/1           iiConvert(int, int, int, sleftv *, sleftv *) [110]
[300]    0.0    0.00    0.00       1         iiDummy(void *) [300]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [33]
[301]    0.0    0.00    0.00       1         iiExport(sleftv *, int) [301]
                0.00    0.00       2/4           idrec::get(char const *, int) [261]
                0.00    0.00       1/988         sleftv::CleanUp(void) [141]
-----------------------------------------------
                0.00    0.00       1/1           iiConvert(int, int, int, sleftv *, sleftv *) [110]
[302]    0.0    0.00    0.00       1         iiI2P(void *) [302]
                0.00    0.00       1/16          p_ISet(int, sip_sring *) [227]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[303]    0.0    0.00    0.00       1         initBuchMoraCrit(skStrategy *) [303]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[304]    0.0    0.00    0.00       1         initBuchMoraPos(skStrategy *) [304]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[305]    0.0    0.00    0.00       1         initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [305]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[306]    0.0    0.00    0.00       1         initMora(sip_sideal *, skStrategy *) [306]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [310]
[307]    0.0    0.00    0.00       1         initRTimer(void) [307]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [310]
[308]    0.0    0.00    0.00       1         initTimer(void) [308]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [310]
[309]    0.0    0.00    0.00       1         init_signals(void) [309]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[310]    0.0    0.00    0.00       1         inits(void) [310]
                0.00    0.00       1/1           init_signals(void) [309]
                0.00    0.00       1/1           initTimer(void) [308]
                0.00    0.00       1/1           initRTimer(void) [307]
                0.00    0.00       1/1           mpsr_Init(void) [316]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[311]    0.0    0.00    0.00       1         jjInitTab1(void) [311]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [119]
[312]    0.0    0.00    0.00       1         jjOPTION_PL(sleftv *, sleftv *) [312]
                0.00    0.00       1/1           setOption(sleftv *, sleftv *) [345]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[313]    0.0    0.00    0.00       1         kMoraUseBucket(skStrategy *) [313]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [43]
[314]    0.0    0.00    0.00       1         m2_end [314]
                0.00    0.00       2/2           fe_reset_input_mode(void) [280]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[315]    0.0    0.00    0.00       1         messageStat(int, int, int, skStrategy *) [315]
                0.00    0.00       1/85          Print [172]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [310]
[316]    0.0    0.00    0.00       1         mpsr_Init(void) [316]
-----------------------------------------------
                0.00    0.00       1/1           feGetExpandedExecutable(void) [292]
[317]    0.0    0.00    0.00       1         omFindExec [317]
                0.00    0.00       1/1           omFindExec_link [318]
                0.00    0.00       1/1           full_readlink [297]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [317]
[318]    0.0    0.00    0.00       1         omFindExec_link [318]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [323]
[319]    0.0    0.00    0.00       1         omFreeKeptAddrFromBin [319]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[320]    0.0    0.00    0.00       1         omGetStickyBinOfBin [320]
-----------------------------------------------
                0.00    0.00       1/1           mmInit(void) [283]
[321]    0.0    0.00    0.00       1         omInitInfo [321]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [323]
[322]    0.0    0.00    0.00       1         omIsKnownTopBin [322]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[323]    0.0    0.00    0.00       1         omMergeStickyBinIntoBin [323]
                0.00    0.00       1/1           _omIsOnList [4344]
                0.00    0.00       1/1           omIsKnownTopBin [322]
                0.00    0.00       1/1           omFreeKeptAddrFromBin [319]
                0.00    0.00       1/1           _omRemoveFromList [4345]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[324]    0.0    0.00    0.00       1         omSizeWOfAddr [324]
-----------------------------------------------
                0.00    0.00       1/1           initSL(sip_sideal *, sip_sideal *, skStrategy *) [69]
[325]    0.0    0.00    0.00       1         pIsConstant(spolyrec *) [325]
-----------------------------------------------
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [129]
[326]    0.0    0.00    0.00       1         pNormalize(spolyrec *) [326]
-----------------------------------------------
                0.00    0.00       1/1           rSetHdl(idrec *, short) [50]
[327]    0.0    0.00    0.00       1         pSetGlobals(sip_sring *, short) [327]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [331]
[328]    0.0    0.00    0.00       1         p_FieldIs(sip_sring *) [328]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[329]    0.0    0.00    0.00       1         p_GetSetmProc(sip_sring *) [329]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [331]
[330]    0.0    0.00    0.00       1         p_OrdIs(sip_sring *) [330]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[331]    0.0    0.00    0.00       1         p_SetProcs(sip_sring *, p_Procs_s *) [331]
                0.00    0.00       1/1           p_FieldIs(sip_sring *) [328]
                0.00    0.00       1/1           p_OrdIs(sip_sring *) [330]
                0.00    0.00       1/1           SetProcs(p_Field, p_Length, p_Ord) [289]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
[332]    0.0    0.00    0.00       1         pairs(skStrategy *) [332]
-----------------------------------------------
                0.00    0.00       1/1           nInitChar(sip_sring *) [48]
[333]    0.0    0.00    0.00       1         rFieldType(sip_sring *) [333]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[334]    0.0    0.00    0.00       1         rGetDivMask(int) [334]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[335]    0.0    0.00    0.00       1         rGetExpSize(unsigned long, int &, int) [335]
-----------------------------------------------
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [339]
[336]    0.0    0.00    0.00       1         rOrd_is_Totaldegree_Ordering(sip_sring *) [336]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [33]
[337]    0.0    0.00    0.00       1         rOrderName(char *) [337]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[338]    0.0    0.00    0.00       1         rRightAdjustVarOffset(sip_sring *) [338]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[339]    0.0    0.00    0.00       1         rSetDegStuff(sip_sring *) [339]
                0.00    0.00       1/1           rOrd_is_Totaldegree_Ordering(sip_sring *) [336]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[340]    0.0    0.00    0.00       1         rSetNegWeight(sip_sring *) [340]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [45]
[341]    0.0    0.00    0.00       1         rSetVarL(sip_sring *) [341]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [44]
[342]    0.0    0.00    0.00       1         rSleftvList2StringArray(sleftv *, char **) [342]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [44]
[343]    0.0    0.00    0.00       1         rSleftvOrdering2Ordering(sleftv *, sip_sring *) [343]
-----------------------------------------------
                0.00    0.00       1/1           updateS(short, skStrategy *) [349]
[344]    0.0    0.00    0.00       1         reorderS(int *, skStrategy *) [344]
-----------------------------------------------
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [312]
[345]    0.0    0.00    0.00       1         setOption(sleftv *, sleftv *) [345]
                0.00    0.00       1/1176        sleftv::Typ(void) [140]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [126]
[346]    0.0    0.00    0.00       1         slInitDBMExtension(s_si_link_extension *) [346]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [126]
[347]    0.0    0.00    0.00       1         slInitMPFileExtension(s_si_link_extension *) [347]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [126]
[348]    0.0    0.00    0.00       1         slInitMPTcpExtension(s_si_link_extension *) [348]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [68]
[349]    0.0    0.00    0.00       1         updateS(short, skStrategy *) [349]
                0.00    0.00       1/1           reorderS(int *, skStrategy *) [344]
-----------------------------------------------
                0.00    0.00     156/156         initPT(void) [100]
[4305]   0.0    0.00    0.00     156         Array<CanonicalForm>::operator[](int) const [4305]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [100]
[4306]   0.0    0.00    0.00      82         Array<CanonicalForm>::~Array(void) [4306]
                0.00    0.00      82/82          __builtin_vec_delete [4307]
-----------------------------------------------
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4306]
[4307]   0.0    0.00    0.00      82         __builtin_vec_delete [4307]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [100]
[4308]   0.0    0.00    0.00      82         Array<CanonicalForm>::Array(void) [4308]
-----------------------------------------------
                0.00    0.00       1/64          yyparse(void) <cycle 1> [33]
                0.00    0.00       1/64          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       2/64          newFile(char *, _IO_FILE *) [127]
                0.00    0.00      60/64          newBuffer(char *, feBufferTypes, procinfo *, int) [65]
[4309]   0.0    0.00    0.00      64         __builtin_new [4309]
-----------------------------------------------
                0.00    0.00       1/63          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/63          sleftv::CleanUp(void) [141]
                0.00    0.00      61/63          exitVoice(void) [180]
[4310]   0.0    0.00    0.00      63         __builtin_delete [4310]
-----------------------------------------------
                0.00    0.00       1/42          global constructors keyed to sattr::Print(void) [4318]
                0.00    0.00       1/42          global constructors keyed to slists_bin [4340]
                0.00    0.00       1/42          global constructors keyed to rnumber_bin [4335]
                0.00    0.00       1/42          global constructors keyed to ip_smatrix_bin [4330]
                0.00    0.00       1/42          global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4341]
                0.00    0.00       1/42          rComplete(sip_sring *, int) [45]
                0.00    0.00       1/42          global constructors keyed to mpsr_sleftv_bin [4333]
                0.00    0.00       1/42          global constructors keyed to kBucketCreate(sip_sring *) [4332]
                0.00    0.00       1/42          global constructors keyed to InternalInteger::InternalInteger_bin [4445]
                0.00    0.00       2/42          global constructors keyed to term::term_bin [4447]
                0.00    0.00       3/42          global constructors keyed to s_si_link_extension_bin [4337]
                0.00    0.00       5/42          global constructors keyed to sSubexpr_bin [4336]
                0.00    0.00       6/42          global constructors keyed to sip_command_bin [4339]
                0.00    0.00      17/42          global constructors keyed to MP_INT_bin [4317]
[4311]   0.0    0.00    0.00      42         _omGetSpecBin [4311]
                0.00    0.00      14/14          _omFindInSortedList [4313]
                0.00    0.00       6/6           _omInsertInSortedList [4314]
-----------------------------------------------
                0.00    0.00      17/17          omAllocNewBinPagesRegion [70]
[4312]   0.0    0.00    0.00      17         _omVallocFromSystem [4312]
-----------------------------------------------
                0.00    0.00      14/14          _omGetSpecBin [4311]
[4313]   0.0    0.00    0.00      14         _omFindInSortedList [4313]
-----------------------------------------------
                0.00    0.00       6/6           _omGetSpecBin [4311]
[4314]   0.0    0.00    0.00       6         _omInsertInSortedList [4314]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4771]
[4315]   0.0    0.00    0.00       1         global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4315]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4771]
[4316]   0.0    0.00    0.00       1         global destructors keyed to gmp_output_digits [4316]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4317]   0.0    0.00    0.00       1         global constructors keyed to MP_INT_bin [4317]
                0.00    0.00      17/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4318]   0.0    0.00    0.00       1         global constructors keyed to sattr::Print(void) [4318]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4319]   0.0    0.00    0.00       1         global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4319]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4320]   0.0    0.00    0.00       1         global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4320]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4321]   0.0    0.00    0.00       1         global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4321]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4322]   0.0    0.00    0.00       1         global constructors keyed to convSingNClapN(snumber *) [4322]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4323]   0.0    0.00    0.00       1         global constructors keyed to dArith2 [4323]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4324]   0.0    0.00    0.00       1         global constructors keyed to feOptSpec [4324]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4325]   0.0    0.00    0.00       1         global constructors keyed to feVersionId [4325]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4326]   0.0    0.00    0.00       1         global constructors keyed to fglmUpdatesource(sip_sideal *) [4326]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4327]   0.0    0.00    0.00       1         global constructors keyed to fglmcomb.cc [4327]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4328]   0.0    0.00    0.00       1         global constructors keyed to gmp_output_digits [4328]
                0.00    0.00       1/2           mmInit(void) [283]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4329]   0.0    0.00    0.00       1         global constructors keyed to iiCurrArgs [4329]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4330]   0.0    0.00    0.00       1         global constructors keyed to ip_smatrix_bin [4330]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4331]   0.0    0.00    0.00       1         global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4331]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4332]   0.0    0.00    0.00       1         global constructors keyed to kBucketCreate(sip_sring *) [4332]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4333]   0.0    0.00    0.00       1         global constructors keyed to mpsr_sleftv_bin [4333]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4334]   0.0    0.00    0.00       1         global constructors keyed to omSingOutOfMemoryFunc [4334]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4335]   0.0    0.00    0.00       1         global constructors keyed to rnumber_bin [4335]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4336]   0.0    0.00    0.00       1         global constructors keyed to sSubexpr_bin [4336]
                0.00    0.00       5/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4337]   0.0    0.00    0.00       1         global constructors keyed to s_si_link_extension_bin [4337]
                0.00    0.00       3/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4338]   0.0    0.00    0.00       1         global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4338]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4339]   0.0    0.00    0.00       1         global constructors keyed to sip_command_bin [4339]
                0.00    0.00       6/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4340]   0.0    0.00    0.00       1         global constructors keyed to slists_bin [4340]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4341]   0.0    0.00    0.00       1         global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4341]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [105]
[4342]   0.0    0.00    0.00       1         global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4342]
-----------------------------------------------
                0.00    0.00       1/1           fe_getopt_long [296]
[4343]   0.0    0.00    0.00       1         _fe_getopt_internal [4343]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [323]
[4344]   0.0    0.00    0.00       1         _omIsOnList [4344]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [323]
[4345]   0.0    0.00    0.00       1         _omRemoveFromList [4345]
-----------------------------------------------

 This table describes the call tree of the program, and was sorted by
 the total amount of time spent in each function and its children.

 Each entry in this table consists of several lines.  The line with the
 index number at the left hand margin lists the current function.
 The lines above it list the functions that called this function,
 and the lines below it list the functions this one called.
 This line lists:
     index	A unique number given to each element of the table.
		Index numbers are sorted numerically.
		The index number is printed next to every function name so
		it is easier to look up where the function in the table.

     % time	This is the percentage of the `total' time that was spent
		in this function and its children.  Note that due to
		different viewpoints, functions excluded by options, etc,
		these numbers will NOT add up to 100%.

     self	This is the total amount of time spent in this function.

     children	This is the total amount of time propagated into this
		function by its children.

     called	This is the number of times the function was called.
		If the function called itself recursively, the number
		only includes non-recursive calls, and is followed by
		a `+' and the number of recursive calls.

     name	The name of the current function.  The index number is
		printed after it.  If the function is a member of a
		cycle, the cycle number is printed between the
		function's name and the index number.


 For the function's parents, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the function into this parent.

     children	This is the amount of time that was propagated from
		the function's children into this parent.

     called	This is the number of times this parent called the
		function `/' the total number of times the function
		was called.  Recursive calls to the function are not
		included in the number after the `/'.

     name	This is the name of the parent.  The parent's index
		number is printed after it.  If the parent is a
		member of a cycle, the cycle number is printed between
		the name and the index number.

 If the parents of the function cannot be determined, the word
 `<spontaneous>' is printed in the `name' field, and all the other
 fields are blank.

 For the function's children, the fields have the following meanings:

     self	This is the amount of time that was propagated directly
		from the child into the function.

     children	This is the amount of time that was propagated from the
		child's children to the function.

     called	This is the number of times the function called
		this child `/' the total number of times the child
		was called.  Recursive calls by the child are not
		listed in the number after the `/'.

     name	This is the name of the child.  The child's index
		number is printed after it.  If the child is a
		member of a cycle, the cycle number is printed
		between the name and the index number.

 If there are any cycles (circles) in the call graph, there is an
 entry for the cycle-as-a-whole.  This entry shows who called the
 cycle (as parents) and the members of the cycle (as children.)
 The `+' recursive calls entry shows the number of function calls that
 were internal to the cycle, and the calls entry for each member shows,
 for that member, how many times it was called from other members of
 the cycle.


Index by function name

 [141] sleftv::CleanUp(void) [148] iiCheckRing(int)       [70] omAllocNewBinPagesRegion (omBinPage.c)
  [80] sleftv::CopyD(int)    [192] iiConvName(char *)    [101] omBinPageIndexFault (omBinPage.c)
 [190] sleftv::Copy(sleftv *) [110] iiConvert(int, int, int, sleftv *, sleftv *) [85] omDoRealloc
 [147] sleftv::Data(void)    [130] iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [317] omFindExec
 [241] sleftv::Eval(void)    [300] iiDummy(void *)       [318] omFindExec_link (omFindExec.c)
 [228] FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [301] iiExport(sleftv *, int) [139] omFreeBinPages
 [254] sLObject::GetpLength(void) [4] iiExprArith1(sleftv *, sleftv *, int) [224] omFreeBinPagesRegion (omBinPage.c)
 [207] HEckeTest(spolyrec *, skStrategy *) [72] iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) [319] omFreeKeptAddrFromBin
 [149] IsCmd(char *, int &)  [242] iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [181] omFreeSizeToSystem
 [287] IsPrime(int)          [119] iiExprArithM(sleftv *, sleftv *, int) [35] omFreeToPageFault
 [169] sleftv::LData(void)   [115] iiGetLibProcBuffer(procinfo *, int) [320] omGetStickyBinOfBin
 [172] Print                 [302] iiI2P(void *)         [274] omGetUsedBinBytes
 [288] PrintLn               [186] iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [159] omGetUsedBytesOfBin (omBin.c)
  [53] PrintS                 [90] iiLibCmd(char *, short) [321] omInitInfo
 [128] sleftv::Print(sleftv *, int) [91] iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [322] omIsKnownTopBin
 [289] SetProcs(p_Field, p_Length, p_Ord) [121] iiMake_proc(idrec *, sleftv *) [323] omMergeStickyBinIntoBin
 [184] sleftv::String(void *, short, int) [71] iiPStart(idrec *, sleftv *) [171] omRealloc0Large
 [140] sleftv::Typ(void)      [82] iiParameter(sleftv *) [163] omReallocSizeFromSystem
 [4306] Array<CanonicalForm>::~Array(void) [255] iiProcArgs(char *, short) [102] omRegisterBinPages (omBinPage.c)
 [4315] global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [210] iiS2Link(void *) [253] omSizeOfAddr
 [4316] global destructors keyed to gmp_output_digits [158] iiTabIndex(sValCmdTab const *, int, int) [156] omSizeOfLargeAddr
 [4317] global constructors keyed to MP_INT_bin [174] iiTestConvert(int, int) [324] omSizeWOfAddr
 [4318] global constructors keyed to sattr::Print(void) [120] iiWRITE(sleftv *, sleftv *) [225] omUnregisterBinPages (omBinPage.c)
 [4319] global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [303] initBuchMoraCrit(skStrategy *) [275] omUpdateInfo
 [4320] global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [304] initBuchMoraPos(skStrategy *) [226] omVfreeToSystem
 [4321] global constructors keyed to idealFunctionals::idealFunctionals(int, int) [68] initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [167] pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
 [4322] global constructors keyed to convSingNClapN(snumber *) [133] initEcartNormal(sLObject *) [26] pDeg(spolyrec *, sip_sring *)
 [4323] global constructors keyed to dArith2 [111] initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [131] pDiff(spolyrec *, int)
 [4324] global constructors keyed to feOptSpec [305] initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [161] pHasNotCF(spolyrec *, spolyrec *)
 [4325] global constructors keyed to feVersionId [96] initL(void) [325] pIsConstant(spolyrec *)
 [104] global constructors keyed to fe_promptstr [306] initMora(sip_sideal *, skStrategy *) [134] pIsHomogeneous(spolyrec *)
 [4326] global constructors keyed to fglmUpdatesource(sip_sideal *) [307] initRTimer(void) [137] pLDeg0c(spolyrec *, int *, sip_sring *)
 [4327] global constructors keyed to fglmcomb.cc [69] initSL(sip_sideal *, sip_sideal *, skStrategy *) [162] pLcm(spolyrec *, spolyrec *, spolyrec *)
 [4328] global constructors keyed to gmp_output_digits [308] initTimer(void) [257] pLength(spolyrec *)
 [4329] global constructors keyed to iiCurrArgs [309] init_signals(void) [16] pNorm(spolyrec *)
 [4330] global constructors keyed to ip_smatrix_bin [87] initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [326] pNormalize(spolyrec *)
 [4331] global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [310] inits(void) [75] pPower(spolyrec *, int)
 [4332] global constructors keyed to kBucketCreate(sip_sring *) [193] ipMoveId(idrec *) [327] pSetGlobals(sip_sring *, short)
 [4333] global constructors keyed to mpsr_sleftv_bin [79] jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [13] p_Add_q__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
 [4334] global constructors keyed to omSingOutOfMemoryFunc [201] jiA_INT(sleftv *, sleftv *, _ssubexpr *) [25] p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
 [4335] global constructors keyed to rnumber_bin [129] jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [27] p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
 [4336] global constructors keyed to sSubexpr_bin [98] jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [328] p_FieldIs(sip_sring *)
 [4337] global constructors keyed to s_si_link_extension_bin [78] jiAssign_1(sleftv *, sleftv *) [329] p_GetSetmProc(sip_sring *)
 [4338] global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [232] jjA_L_LIST(sleftv *, sleftv *) [15] p_GetShortExpVector(spolyrec *, sip_sring *)
 [4339] global constructors keyed to sip_command_bin [243] jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [227] p_ISet(int, sip_sring *)
 [4340] global constructors keyed to slists_bin [211] jjCOUNT_L(sleftv *, sleftv *) [215] p_IsPurePower(spolyrec *, sip_sring *)
 [4341] global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [223] jjDEFINED(sleftv *, sleftv *) [266] p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
 [4342] global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [212] jjGT_I(sleftv *, sleftv *, sleftv *) [12] p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPos__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
  [58] _IO_do_write          [262] jjINDEX_I(sleftv *, sleftv *, sleftv *) [234] p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
  [59] _IO_file_sync         [311] jjInitTab1(void)      [330] p_OrdIs(sip_sring *)
  [60] _IO_file_write        [132] jjJACOB_P(sleftv *, sleftv *) [331] p_SetProcs(sip_sring *, p_Procs_s *)
  [41] _IO_file_xsputn       [213] jjKLAMMER_PL(sleftv *, sleftv *) [150] p_Setm_TotalDegree(spolyrec *, sip_sring *)
  [61] _IO_str_init_static   [272] jjMEMORY(sleftv *, sleftv *) [22] p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
 [106] Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [229] jjMINUS_I(sleftv *, sleftv *, sleftv *) [17] p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPos__FP7kBucket
 [4310] __builtin_delete     [263] jjMakeSub(sleftv *)   [332] pairs(skStrategy *)
 [4309] __builtin_new        [312] jjOPTION_PL(sleftv *, sleftv *) [63] pmInit(char *, short &)
 [4307] __builtin_vec_delete [230] jjOP_REST(sleftv *, sleftv *, sleftv *) [196] namerec::pop(short)
  [99] __builtin_vec_new     [166] jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [20] posInL17(sLObject *, int, sLObject *, skStrategy *)
 (4917) __mcount_internal    [108] jjPLUS_P(sleftv *, sleftv *, sleftv *) [222] posInS(spolyrec **, int, spolyrec *)
 [4308] Array<CanonicalForm>::Array(void) [170] jjPLUS_S(sleftv *, sleftv *, sleftv *) [145] posInT17(sTObject *, int, sLObject const &)
 [107] Array<CanonicalForm>::Array(int) [74] jjPOWER_P(sleftv *, sleftv *, sleftv *) [14] pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
 [4305] Array<CanonicalForm>::operator[](int) const [231] jjPROC1(sleftv *, sleftv *) [124] namerec::push(sip_package *, char *, int, short)
 [4343] _fe_getopt_internal  [214] jjPROC(sleftv *, sleftv *, sleftv *) [45] rComplete(sip_sring *, int)
 [4313] _omFindInSortedList  [244] jjREAD(sleftv *, sleftv *) [276] rDBTest(sip_sring *, char *, int)
 [4311] _omGetSpecBin          [5] jjSTD(sleftv *, sleftv *) [164] rEALLOc
 [4314] _omInsertInSortedList [189] jjSTRING_PL(sleftv *, sleftv *) [333] rFieldType(sip_sring *)
 [4344] _omIsOnList          [202] jjSYSTEM(sleftv *, sleftv *) [334] rGetDivMask(int)
 [4345] _omRemoveFromList     [77] jjTIMES_P(sleftv *, sleftv *, sleftv *) [335] rGetExpSize(unsigned long, int &, int)
 [4312] _omVallocFromSystem   [32] kBucketCanonicalize(kBucket *) [44] rInit(sleftv *, sleftv *, sleftv *)
 [290] atGet(sleftv *, char *) [37] kBucketClear(kBucket *, spolyrec **, int *) [185] rIsPolyVar(int, sip_sring *)
 [271] calloc                 [55] kBucketCreate(sip_sring *) [165] rIsRingVar(char *)
 [203] cancelunit(sLObject *) [143] kBucketDestroy(kBucket **) [336] rOrd_is_Totaldegree_Ordering(sip_sring *)
 [109] chainCrit(spolyrec *, int, skStrategy *) [144] kBucketInit(kBucket *, spolyrec *, int) [337] rOrderName(char *)
  [31] cleanT(skStrategy *)   [11] kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [338] rRightAdjustVarOffset(sip_sring *)
 [153] copy_string(lp_modes)  [54] kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [339] rSetDegStuff(sip_sring *)
 [151] current_pos(int)       [19] kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [50] rSetHdl(idrec *, short)
 [258] deleteHC(sLObject *, skStrategy *, short) [313] kMoraUseBucket(skStrategy *) [340] rSetNegWeight(sip_sring *)
 [175] deleteInL(sLObject *, int *, int, skStrategy *) [6] kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [341] rSetVarL(sip_sring *)
   [9] doRed(sLObject *, sTObject *, short, skStrategy *) [176] killhdl(idrec *, idrec **) [342] rSleftvList2StringArray(sleftv *, char **)
 [259] eati(char *, int *)   [194] killlocals(int)       [343] rSleftvOrdering2Ordering(sleftv *, sip_sring *)
  [51] enterL(sLObject **, int *, int *, sLObject, int) [118] ksCheckCoeff(snumber **, snumber **) [8] redEcart(sLObject *, skStrategy *)
  [89] enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [114] ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [216] redtail(spolyrec *, int, skStrategy *)
 [208] enterSBba(sLObject, int, skStrategy *, int) [67] ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [284] reinit_yylp(void)
 [209] enterSMora(sLObject, int, skStrategy *, int) [10] ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [344] reorderS(int *, skStrategy *)
  [21] enterT(sLObject, skStrategy *, int) [237] lRingDependend(slists *) [345] setOption(sleftv *, sleftv *)
 [122] enterid(char *, int, int, idrec **, short) [238] libread(_IO_FILE *, char *, int) [123] idrec::set(char *, int, int, short)
  [83] enterpairs(spolyrec *, int, int, int, skStrategy *, int) [154] sleftv::listLength(void) [217] slCleanUp(sip_link *)
  [30] exitBuchMora(skStrategy *) [314] m2_end           [218] slCloseAscii(sip_link *)
 [191] exitBuffer(feBufferTypes) [173] mALLOc            [346] slInitDBMExtension(s_si_link_extension *)
 [180] exitVoice(void)         [1] main                  [347] slInitMPFileExtension(s_si_link_extension *)
 [291] exprlist_length(sleftv *) [264] make_version(char *, int) [348] slInitMPTcpExtension(s_si_link_extension *)
 [160] fREe                   [86] malloc                [219] slInit(sip_link *, char *)
 [249] feCleanResourceValue(feResourceType, char *) [233] malloc_extend_top (omMalloc.c) [81] slInternalCopy(sleftv *, int, void *, _ssubexpr *)
 [188] feCleanUpFile(char *) [273] malloc_update_mallinfo [220] slKill(sip_link *)
 [277] feCleanUpPath(char *) [155] malloc_usable_size    [221] slOpenAscii(sip_link *, short)
 [113] feFopen(char *, char *, char *, int, int) (2368) mcount [245] slReadAscii2(sip_link *, sleftv *)
 [292] feGetExpandedExecutable(void) [36] memmove        [246] slReadAscii(sip_link *)
 [278] feGetOptIndex(char const *) [315] messageStat(int, int, int, skStrategy *) [247] slRead(sip_link *, sleftv *)
 [205] feGetResourceConfig(char) [42] message(int, int *, int *, skStrategy *) [126] slStandardInit(void)
 [250] feInitResource(feResourceConfig_s *, int) [283] mmInit(void) [235] slWriteAscii(sip_link *, sleftv *)
 [293] feInitResources(char *) [57] mmap                 [236] slWrite(sip_link *, sleftv *)
 [294] feOptAction(feOptIndex) [7] mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [62] syMake(sleftv *, char *, idrec *)
 [152] fePrintEcho(char *, char *) [316] mpsr_Init(void) [349] updateS(short, skStrategy *)
  [93] feReadLine(char *, int) [199] myfread(void *, unsigned int, unsigned int, _IO_FILE *) [18] write
 [206] feResource(feResourceConfig_s *, int) [265] mystrcpy(char *, char *) [103] yy_create_buffer(_IO_FILE *, int)
 [240] feResource(char, int)  [66] myynewbuffer(void)     [64] yy_flex_alloc(unsigned int)
 [295] feSetOptValue(feOptIndex, int) [195] myyoldbuffer(void *) [95] yy_flex_alloc(unsigned int)
 [260] feSprintf(char *, char const *, int) [24] nDummy1(snumber **) [179] yy_flex_free(void *)
 [251] feVerifyResourceValue(feResourceType, char *) [38] nDummy2(snumber *&) [267] yy_flex_free(void *)
 [296] fe_getopt_long         [48] nInitChar(sip_sring *) [177] yy_flush_buffer(yy_buffer_state *)
 [279] fe_reset_fe            [49] nSetChar(sip_sring *, short) [94] yy_get_next_buffer(void)
 [280] fe_reset_input_mode(void) [182] ndCopy(snumber *) [239] yy_get_next_buffer(void)
  [39] fork                  [198] ndGcd(snumber *, snumber *) [146] yy_get_previous_state(void)
 [187] free                   [65] newBuffer(char *, feBufferTypes, procinfo *, int) [248] yy_get_previous_state(void)
 [281] freeSize              [127] newFile(char *, _IO_FILE *) [178] yy_init_buffer(yy_buffer_state *, _IO_FILE *)
 [297] full_readlink (omFindExec.c) [34] npDiv(snumber *, snumber *) [168] yy_load_buffer_state(void)
  [40] fwrite                [256] npEqual(snumber *, snumber *) [43] yylex(MYYSTYPE *)
 [282] getTimer(void)         [46] npInitChar(int, sip_sring *) [97] yylp_create_buffer(_IO_FILE *, int)
 [261] idrec::get(char const *, int) [142] npInit(int)   [268] yylp_flush_buffer(yy_buffer_state *)
 [157] ggetid(char const *, short) [29] npIsOne(snumber *) [269] yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
  [84] idCopy(sip_sideal *)  [200] npIsZero(snumber *)   [270] yylp_load_buffer_state(void)
 [135] idHomIdeal(sip_sideal *, sip_sideal *) [204] npMult(snumber *, snumber *) [92] yylplex(char *, char *, lib_style_types *, lp_modes)
 [252] idInit(int, int)       [52] npNeg(snumber *)      [285] yylprestart(_IO_FILE *)
 [298] idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [183] npRead(char *, snumber **) [286] yylpwrap
 [299] idSkipZeroes(sip_sideal *) [47] npSetChar(int, sip_sring *) [33] yyparse(void)
  [76] id_Delete(sip_sideal **, sip_sring *) [23] omAllocBinFromFullPage [197] yyrestart(_IO_FILE *)
  [73] iiAssign(sleftv *, sleftv *) [28] omAllocBinPage    [3] <cycle 1>
 [125] iiCheckNest(void)      [56] omAllocFromSystem     [138] <cycle 2>
