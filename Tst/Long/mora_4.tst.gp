Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 70.99     53.04    53.04   243360     0.22     0.23  p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
  9.21     59.92     6.88    70422     0.10     0.10  p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
  3.41     62.47     2.55     3131     0.81    16.26  redEcart(sLObject *, skStrategy *)
  2.36     64.23     1.76                             __mcount_internal
  2.02     65.74     1.51      463     3.26     3.87  pNorm(spolyrec *)
  1.30     66.71     0.97                             mcount
  1.06     67.50     0.79    98945     0.01     0.01  pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec
  1.00     68.25     0.75   189096     0.00     0.00  pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
  0.98     68.98     0.73      470     1.55     1.65  p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
  0.78     69.56     0.58      818     0.71     0.81  p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
  0.76     70.13     0.57   333279     0.00     0.00  kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int)
  0.63     70.60     0.47   333047     0.00     0.00  p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket
  0.60     71.05     0.45     7524     0.06     0.06  omAllocBinPage
  0.52     71.44     0.39   247738     0.00     0.00  omFreeToPageFault
  0.51     71.82     0.38   332946     0.00     0.19  ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *)
  0.41     72.13     0.31    60941     0.01     0.01  pLength(spolyrec *)
  0.40     72.43     0.30   243268     0.00     0.00  omAllocBinFromFullPage
  0.36     72.70     0.27      644     0.42     0.89  deleteHC(sLObject *, skStrategy *, short)
  0.35     72.96     0.26   339251     0.00     0.00  p_GetShortExpVector(spolyrec *, sip_sring *)
  0.33     73.21     0.25      618     0.40     0.43  p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
  0.28     73.42     0.21   220430     0.00     0.00  posInL17(sLObject *, int, sLObject *, skStrategy *)
  0.21     73.58     0.16   333047     0.00     0.19  kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *)
  0.19     73.72     0.14                             write
  0.16     73.84     0.12   923651     0.00     0.00  npDiv(snumber *, snumber *)
  0.16     73.96     0.12     3161     0.04     6.15  redFirst(sLObject *, skStrategy *)
  0.13     74.06     0.10  1258208     0.00     0.00  nDummy1(snumber **)
  0.12     74.15     0.09  1257408     0.00     0.00  npIsOne(snumber *)
  0.12     74.24     0.09   334636     0.00     0.00  pDeg(spolyrec *, sip_sring *)
  0.11     74.32     0.08      180     0.44     0.44  pLDeg0c(spolyrec *, int *, sip_sring *)
  0.05     74.36     0.04                             fflush
  0.05     74.40     0.04                             fork
  0.04     74.43     0.03     5906     0.01     0.01  kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *)
  0.04     74.46     0.03                             _IO_file_write
  0.03     74.48     0.02   924082     0.00     0.00  nDummy2(snumber *&)
  0.03     74.50     0.02       33     0.61     3.09  yyparse(void)
  0.03     74.52     0.02        1    20.00    20.14  npInitChar(int, sip_sring *)
  0.03     74.54     0.02                             _IO_file_xsputn
  0.03     74.56     0.02                             memmove
  0.01     74.57     0.01     5522     0.00     0.00  omFreeBinPages
  0.01     74.58     0.01     1288     0.01     0.48  kBucketCanonicalize(kBucket *)
  0.01     74.59     0.01      965     0.01     0.01  sleftv::CleanUp(void)
  0.01     74.60     0.01      782     0.01     0.01  feReadLine(char *, int)
  0.01     74.61     0.01      470     0.02     1.67  enterT(sLObject, skStrategy *, int)
  0.01     74.62     0.01      141     0.07     0.07  mALLOc
  0.01     74.63     0.01       65     0.15     0.15  jiA_STRING(sleftv *, sleftv *, _ssubexpr *)
  0.01     74.64     0.01        2     5.00     5.07  yylplex(char *, char *, lib_style_types *, lp_modes)
  0.01     74.65     0.01        1    10.00   192.02  cleanT(skStrategy *)
  0.01     74.66     0.01        1    10.00    10.14  npSetChar(int, sip_sring *)
  0.01     74.67     0.01                             _IO_do_write
  0.01     74.68     0.01                             flockfile
  0.01     74.69     0.01                             fwrite
  0.01     74.70     0.01                             memset
  0.01     74.71     0.01                             strcat
  0.01     74.72     0.01                             system
  0.00     74.72     0.00   233025     0.00     0.20  doRed(sLObject *, sTObject *, short, skStrategy *)
  0.00     74.72     0.00   179398     0.00     0.00  npNeg(snumber *)
  0.00     74.72     0.00     7047     0.00     0.00  enterL(sLObject **, int *, int *, sLObject, int)
  0.00     74.72     0.00     6304     0.00     0.00  PrintS
  0.00     74.72     0.00     6292     0.00     0.00  message(int, int *, int *, skStrategy *)
  0.00     74.72     0.00     3633     0.00     0.00  p_Setm_TotalDegree(spolyrec *, sip_sring *)
  0.00     74.72     0.00     1788     0.00     0.01  yylex(MYYSTYPE *)
  0.00     74.72     0.00     1587     0.00     0.00  enterOnePair(int, spolyrec *, int, int, skStrategy *, int)
  0.00     74.72     0.00     1587     0.00     0.00  pHasNotCF(spolyrec *, spolyrec *)
  0.00     74.72     0.00     1587     0.00     0.00  pLcm(spolyrec *, spolyrec *, spolyrec *)
  0.00     74.72     0.00     1362     0.00     0.00  rIsPolyVar(int, sip_sring *)
  0.00     74.72     0.00     1297     0.00     0.00  pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
  0.00     74.72     0.00     1147     0.00     0.00  sleftv::Typ(void)
  0.00     74.72     0.00      782     0.00     0.01  yy_get_next_buffer(void)
  0.00     74.72     0.00      766     0.00     0.00  yy_get_previous_state(void)
  0.00     74.72     0.00      697     0.00     0.00  kBucketInit(kBucket *, spolyrec *, int)
  0.00     74.72     0.00      676     0.00     0.00  initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int)
  0.00     74.72     0.00      676     0.00     0.00  ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *)
  0.00     74.72     0.00      644     0.00     0.48  kBucketClear(kBucket *, spolyrec **, int *)
  0.00     74.72     0.00      602     0.00     0.00  npInit(int)
  0.00     74.72     0.00      563     0.00     0.00  sleftv::Data(void)
  0.00     74.72     0.00      563     0.00     0.00  deleteInL(sLObject *, int *, int, skStrategy *)
  0.00     74.72     0.00      545     0.00     0.00  iiCheckRing(int)
  0.00     74.72     0.00      533     0.00     0.00  cancelunit(sLObject *)
  0.00     74.72     0.00      523     0.00     0.00  kBucketCreate(sip_sring *)
  0.00     74.72     0.00      523     0.00     0.00  kBucketDestroy(kBucket **)
  0.00     74.72     0.00      502     0.00     0.00  IsCmd(char *, int &)
  0.00     74.72     0.00      467     0.00     0.00  posInT17(sTObject *, int, sLObject const &)
  0.00     74.72     0.00      425     0.00     0.00  hStepS(long **, int, int *, int, int *, long *)
  0.00     74.72     0.00      403     0.00     0.00  current_pos(int)
  0.00     74.72     0.00      375     0.00     0.00  fePrintEcho(char *, char *)
  0.00     74.72     0.00      354     0.00     0.00  hHedge(spolyrec *)
  0.00     74.72     0.00      354     0.00     0.00  hPure(long **, int, int *, int *, int, long *, int *)
  0.00     74.72     0.00      350     0.00     0.00  hElimS(long **, int *, int, int, int *, int)
  0.00     74.72     0.00      350     0.00     0.00  hLex2S(long **, int, int, int, int *, int, long **)
  0.00     74.72     0.00      347     0.00     0.00  copy_string(lp_modes)
  0.00     74.72     0.00      340     0.00     0.00  sleftv::listLength(void)
  0.00     74.72     0.00      320     0.00     0.00  ggetid(char const *, short)
  0.00     74.72     0.00      320     0.00     0.06  syMake(sleftv *, char *, idrec *)
  0.00     74.72     0.00      232     0.00     0.00  iiTabIndex(sValCmdTab const *, int, int)
  0.00     74.72     0.00      226     0.00     0.00  ndCopy(snumber *)
  0.00     74.72     0.00      197     0.00     0.00  p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
  0.00     74.72     0.00      189     0.00     0.00  malloc_usable_size
  0.00     74.72     0.00      186     0.00     0.00  omSizeOfLargeAddr
  0.00     74.72     0.00      177     0.00     0.00  p_LmCmp(spolyrec *, spolyrec *, sip_sring *)
  0.00     74.72     0.00      167     0.00     0.00  __builtin_vec_new
  0.00     74.72     0.00      165     0.00     0.05  iiExprArith2(sleftv *, sleftv *, int, sleftv *, short)
  0.00     74.72     0.00      162     0.00     0.00  omGetUsedBytesOfBin
  0.00     74.72     0.00      156     0.00     0.00  Array<CanonicalForm>::operator[](int) const
  0.00     74.72     0.00      154     0.00     0.00  fREe
  0.00     74.72     0.00      130     0.00     0.00  npMult(snumber *, snumber *)
  0.00     74.72     0.00      126     0.00     0.00  Print
  0.00     74.72     0.00      122     0.00     0.00  sleftv::CopyD(int)
  0.00     74.72     0.00      120     0.00     0.00  enterid(char *, int, int, idrec **, short)
  0.00     74.72     0.00      120     0.00     0.00  idrec::set(char *, int, int, short)
  0.00     74.72     0.00      117     0.00     0.00  rIsRingVar(char *)
  0.00     74.72     0.00      115     0.00     0.02  omReallocSizeFromSystem
  0.00     74.72     0.00      115     0.00     0.02  rEALLOc
  0.00     74.72     0.00      113     0.00     0.00  ksCheckCoeff(snumber **, snumber **)
  0.00     74.72     0.00      113     0.00     0.20  ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **)
  0.00     74.72     0.00      113     0.00     0.00  ndGcd(snumber *, snumber *)
  0.00     74.72     0.00      109     0.00     0.07  omAllocFromSystem
  0.00     74.72     0.00      108     0.00     0.00  jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00      103     0.00     0.13  iiAssign(sleftv *, sleftv *)
  0.00     74.72     0.00       97     0.00     0.00  yy_load_buffer_state(void)
  0.00     74.72     0.00       91     0.00     0.00  sleftv::LData(void)
  0.00     74.72     0.00       91     0.00     0.12  jiAssign_1(sleftv *, sleftv *)
  0.00     74.72     0.00       90     0.00     0.00  jjPLUS_S(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       88     0.00     0.01  iiExprArithM(sleftv *, sleftv *, int)
  0.00     74.72     0.00       82     0.00     0.00  Array<CanonicalForm>::~Array(void)
  0.00     74.72     0.00       82     0.00     0.00  Array<CanonicalForm>::operator=(Array<CanonicalForm> const &)
  0.00     74.72     0.00       82     0.00     0.00  __builtin_vec_delete
  0.00     74.72     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(void)
  0.00     74.72     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(int)
  0.00     74.72     0.00       77     0.00     0.01  iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short)
  0.00     74.72     0.00       76     0.00     0.00  iiTestConvert(int, int)
  0.00     74.72     0.00       75     0.00     0.00  hGetmem(int, long **, monrec *)
  0.00     74.72     0.00       75     0.00     0.00  hGetpure(long *)
  0.00     74.72     0.00       72     0.00     0.00  iiConvert(int, int, int, sleftv *, sleftv *)
  0.00     74.72     0.00       69     0.00     0.00  killhdl(idrec *, idrec **)
  0.00     74.72     0.00       67     0.00  1067.48  iiExprArith1(sleftv *, sleftv *, int)
  0.00     74.72     0.00       66     0.00     0.04  yy_flex_alloc(unsigned int)
  0.00     74.72     0.00       65     0.00     0.00  yy_flush_buffer(yy_buffer_state *)
  0.00     74.72     0.00       65     0.00     0.00  yy_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     74.72     0.00       64     0.00     0.00  __builtin_new
  0.00     74.72     0.00       64     0.00     0.00  yy_flex_free(void *)
  0.00     74.72     0.00       63     0.00     0.00  HEckeTest(spolyrec *, skStrategy *)
  0.00     74.72     0.00       63     0.00     0.00  __builtin_delete
  0.00     74.72     0.00       63     0.00     0.00  chainCrit(spolyrec *, int, skStrategy *)
  0.00     74.72     0.00       63     0.00     0.00  enterSBba(sLObject, int, skStrategy *, int)
  0.00     74.72     0.00       63     0.00     9.14  enterSMora(sLObject, int, skStrategy *, int)
  0.00     74.72     0.00       63     0.00     0.02  enterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     74.72     0.00       63     0.00     0.02  initenterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     74.72     0.00       63     0.00     0.00  p_IsPurePower(spolyrec *, sip_sring *)
  0.00     74.72     0.00       63     0.00     0.00  redtail(spolyrec *, int, skStrategy *)
  0.00     74.72     0.00       62     0.00     0.02  omDoRealloc
  0.00     74.72     0.00       62     0.00     0.00  posInS(spolyrec **, int, spolyrec *)
  0.00     74.72     0.00       61     0.00     0.00  exitVoice(void)
  0.00     74.72     0.00       61     0.00     0.00  omFreeSizeToSystem
  0.00     74.72     0.00       60     0.00     0.04  newBuffer(char *, feBufferTypes, procinfo *, int)
  0.00     74.72     0.00       58     0.00     0.00  npEqual(snumber *, snumber *)
  0.00     74.72     0.00       57     0.00     0.00  _omVallocFromSystem
  0.00     74.72     0.00       57     0.00     0.09  omAllocNewBinPagesRegion
  0.00     74.72     0.00       57     0.00     0.02  omBinPageIndexFault
  0.00     74.72     0.00       57     0.00     0.02  omRegisterBinPages
  0.00     74.72     0.00       49     0.00     0.00  npRead(char *, snumber **)
  0.00     74.72     0.00       49     0.00     0.42  pmInit(char *, short &)
  0.00     74.72     0.00       45     0.00     0.11  iiParameter(sleftv *)
  0.00     74.72     0.00       44     0.00     0.00  sleftv::String(void *, short, int)
  0.00     74.72     0.00       42     0.00     0.00  _omGetSpecBin
  0.00     74.72     0.00       42     0.00     0.02  omRealloc0Large
  0.00     74.72     0.00       41     0.00     0.00  iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short)
  0.00     74.72     0.00       36     0.00     0.01  malloc
  0.00     74.72     0.00       34     0.00     0.00  free
  0.00     74.72     0.00       33     0.00     0.00  feCleanUpFile(char *)
  0.00     74.72     0.00       33     0.00     0.00  jjSTRING_PL(sleftv *, sleftv *)
  0.00     74.72     0.00       33     0.00     0.00  namerec::push(sip_package *, char *, int, short)
  0.00     74.72     0.00       32     0.00     0.00  sleftv::Copy(sleftv *)
  0.00     74.72     0.00       32     0.00     0.00  exitBuffer(feBufferTypes)
  0.00     74.72     0.00       32     0.00     0.00  iiCheckNest(void)
  0.00     74.72     0.00       32     0.00     0.00  iiConvName(char *)
  0.00     74.72     0.00       32     0.00     0.00  iiMake_proc(idrec *, sleftv *)
  0.00     74.72     0.00       32     0.00     0.04  iiPStart(idrec *, sleftv *)
  0.00     74.72     0.00       32     0.00     0.00  ipMoveId(idrec *)
  0.00     74.72     0.00       32     0.00     0.00  killlocals(int)
  0.00     74.72     0.00       32     0.00     0.07  myynewbuffer(void)
  0.00     74.72     0.00       32     0.00     0.00  myyoldbuffer(void *)
  0.00     74.72     0.00       32     0.00     0.00  namerec::pop(short)
  0.00     74.72     0.00       32     0.00     0.00  yyrestart(_IO_FILE *)
  0.00     74.72     0.00       28     0.00     0.00  npIsZero(snumber *)
  0.00     74.72     0.00       26     0.00     0.01  sleftv::Print(sleftv *, int)
  0.00     74.72     0.00       26     0.00     0.00  myfread(void *, unsigned int, unsigned int, _IO_FILE *)
  0.00     74.72     0.00       25     0.00     0.00  jjSYSTEM(sleftv *, sleftv *)
  0.00     74.72     0.00       23     0.00     0.00  jiA_INT(sleftv *, sleftv *, _ssubexpr *)
  0.00     74.72     0.00       20     0.00     0.00  feGetResourceConfig(char)
  0.00     74.72     0.00       20     0.00     0.00  feResource(feResourceConfig_s *, int)
  0.00     74.72     0.00       18     0.00     0.00  iiS2Link(void *)
  0.00     74.72     0.00       18     0.00     0.00  jjCOUNT_L(sleftv *, sleftv *)
  0.00     74.72     0.00       18     0.00     0.00  jjGT_I(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       18     0.00     0.00  jjKLAMMER_PL(sleftv *, sleftv *)
  0.00     74.72     0.00       18     0.00     0.00  jjPROC(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       18     0.00     0.00  omFreeBinPagesRegion
  0.00     74.72     0.00       18     0.00     0.00  omUnregisterBinPages
  0.00     74.72     0.00       18     0.00     0.00  omVfreeToSystem
  0.00     74.72     0.00       18     0.00     0.00  slCleanUp(sip_link *)
  0.00     74.72     0.00       18     0.00     0.00  slCloseAscii(sip_link *)
  0.00     74.72     0.00       18     0.00     0.00  slInit(sip_link *, char *)
  0.00     74.72     0.00       18     0.00     0.00  slKill(sip_link *)
  0.00     74.72     0.00       18     0.00     0.00  slOpenAscii(sip_link *, short)
  0.00     74.72     0.00       16     0.00     0.00  sLObject::GetpLength(void)
  0.00     74.72     0.00       16     0.00     0.00  jjDEFINED(sleftv *, sleftv *)
  0.00     74.72     0.00       14     0.00     0.00  FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &)
  0.00     74.72     0.00       14     0.00     0.00  _omFindInSortedList
  0.00     74.72     0.00       14     0.00     0.00  jjPROC1(sleftv *, sleftv *)
  0.00     74.72     0.00       13     0.00     0.00  feFopen(char *, char *, char *, int, int)
  0.00     74.72     0.00       12     0.00     0.01  jjA_L_LIST(sleftv *, sleftv *)
  0.00     74.72     0.00       11     0.00     0.01  iiWRITE(sleftv *, sleftv *)
  0.00     74.72     0.00       11     0.00     0.00  jjMINUS_I(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       11     0.00     0.00  malloc_extend_top
  0.00     74.72     0.00       11     0.00     0.00  p_ISet(int, sip_sring *)
  0.00     74.72     0.00       11     0.00     0.00  slWriteAscii(sip_link *, sleftv *)
  0.00     74.72     0.00       11     0.00     0.00  slWrite(sip_link *, sleftv *)
  0.00     74.72     0.00       10     0.00     0.00  jjOP_REST(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       10     0.00     0.17  jjPOWER_P(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00       10     0.00     0.17  pPower(spolyrec *, int)
  0.00     74.72     0.00        9     0.00     0.00  lRingDependend(slists *)
  0.00     74.72     0.00        9     0.00     0.00  libread(_IO_FILE *, char *, int)
  0.00     74.72     0.00        9     0.00     0.00  yy_get_next_buffer(void)
  0.00     74.72     0.00        8     0.00     0.00  feResource(char, int)
  0.00     74.72     0.00        8     0.00     0.00  slInternalCopy(sleftv *, int, void *, _ssubexpr *)
  0.00     74.72     0.00        7     0.00     0.00  sleftv::Eval(void)
  0.00     74.72     0.00        7     0.00     0.03  iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        7     0.00     0.00  jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        7     0.00     0.00  jjREAD(sleftv *, sleftv *)
  0.00     74.72     0.00        7     0.00     0.00  slReadAscii2(sip_link *, sleftv *)
  0.00     74.72     0.00        7     0.00     0.00  slReadAscii(sip_link *)
  0.00     74.72     0.00        7     0.00     0.00  slRead(sip_link *, sleftv *)
  0.00     74.72     0.00        7     0.00     0.00  yy_get_previous_state(void)
  0.00     74.72     0.00        6     0.00     0.00  _omInsertInSortedList
  0.00     74.72     0.00        6     0.00     0.00  feCleanResourceValue(feResourceType, char *)
  0.00     74.72     0.00        6     0.00     0.00  feInitResource(feResourceConfig_s *, int)
  0.00     74.72     0.00        6     0.00     0.00  feVerifyResourceValue(feResourceType, char *)
  0.00     74.72     0.00        6     0.00     0.10  jjPLUS_P(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        6     0.00     0.43  jjTIMES_P(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        6     0.00     0.00  omSizeOfAddr
  0.00     74.72     0.00        6     0.00     0.00  p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
  0.00     74.72     0.00        5     0.00     0.00  idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *)
  0.00     74.72     0.00        5     0.00     0.00  iiGetLibProcBuffer(procinfo *, int)
  0.00     74.72     0.00        5     0.00     0.00  iiProcArgs(char *, short)
  0.00     74.72     0.00        4     0.00     0.00  feSprintf(char *, char const *, int)
  0.00     74.72     0.00        4     0.00   104.10  firstUpdate(skStrategy *)
  0.00     74.72     0.00        4     0.00     0.00  idrec::get(char const *, int)
  0.00     74.72     0.00        4     0.00     0.00  hCreate(int)
  0.00     74.72     0.00        4     0.00     0.00  hDelete(long **, int)
  0.00     74.72     0.00        4     0.00     0.00  hHedgeStep(long *, long **, int, int *, int, spolyrec *)
  0.00     74.72     0.00        4     0.00     0.00  hInit(sip_sideal *, sip_sideal *, int *, sip_sring *)
  0.00     74.72     0.00        4     0.00     0.00  hKill(monrec **, int)
  0.00     74.72     0.00        4     0.00     0.00  hLexS(long **, int, int *, int)
  0.00     74.72     0.00        4     0.00     0.00  hOrdSupp(long **, int, int *, int)
  0.00     74.72     0.00        4     0.00     0.00  hStaircase(long **, int *, int *, int)
  0.00     74.72     0.00        4     0.00     0.00  idInit(int, int)
  0.00     74.72     0.00        4     0.00     0.00  jjINDEX_I(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        4     0.00     0.00  jjMakeSub(sleftv *)
  0.00     74.72     0.00        4     0.00     0.00  make_version(char *, int)
  0.00     74.72     0.00        4     0.00     0.00  mystrcpy(char *, char *)
  0.00     74.72     0.00        4     0.00     0.44  newHEdge(spolyrec **, int, skStrategy *)
  0.00     74.72     0.00        4     0.00     0.00  pEnlargeSet(spolyrec ***, int, int)
  0.00     74.72     0.00        4     0.00     0.04  reorderL(skStrategy *)
  0.00     74.72     0.00        4     0.00     0.00  scComputeHC(sip_sideal *, int, spolyrec *&)
  0.00     74.72     0.00        4     0.00    39.39  updateLHC(skStrategy *)
  0.00     74.72     0.00        4     0.00     0.04  yy_flex_alloc(unsigned int)
  0.00     74.72     0.00        4     0.00     0.00  yy_flex_free(void *)
  0.00     74.72     0.00        4     0.00     0.00  yylp_flush_buffer(yy_buffer_state *)
  0.00     74.72     0.00        4     0.00     0.00  yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     74.72     0.00        4     0.00     0.00  yylp_load_buffer_state(void)
  0.00     74.72     0.00        3     0.00     0.00  calloc
  0.00     74.72     0.00        3     0.00     0.44  initEcartNormal(sLObject *)
  0.00     74.72     0.00        3     0.00     0.00  jjMEMORY(sleftv *, sleftv *)
  0.00     74.72     0.00        3     0.00     0.00  malloc_update_mallinfo
  0.00     74.72     0.00        3     0.00     0.00  omGetUsedBinBytes
  0.00     74.72     0.00        3     0.00     0.00  omUpdateInfo
  0.00     74.72     0.00        3     0.00     0.00  pDiff(spolyrec *, int)
  0.00     74.72     0.00        3     0.00     0.00  pIsConstant(spolyrec *)
  0.00     74.72     0.00        3     0.00     0.00  posInT2(sTObject *, int, sLObject const &)
  0.00     74.72     0.00        3     0.00     0.00  rDBTest(sip_sring *, char *, int)
  0.00     74.72     0.00        2     0.00     0.00  ZeroOrd_2_NonZeroOrd(p_Ord, int)
  0.00     74.72     0.00        2     0.00     0.00  eati(char *, int *)
  0.00     74.72     0.00        2     0.00     0.00  feCleanUpPath(char *)
  0.00     74.72     0.00        2     0.00     0.00  feGetOptIndex(char const *)
  0.00     74.72     0.00        2     0.00     0.00  fe_reset_fe
  0.00     74.72     0.00        2     0.00     0.00  fe_reset_input_mode(void)
  0.00     74.72     0.00        2     0.00     0.00  freeSize
  0.00     74.72     0.00        2     0.00     0.00  getTimer(void)
  0.00     74.72     0.00        2     0.00     0.43  id_Delete(sip_sideal **, sip_sring *)
  0.00     74.72     0.00        2     0.00     5.07  iiLibCmd(char *, short)
  0.00     74.72     0.00        2     0.00     5.07  iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short)
  0.00     74.72     0.00        2     0.00     0.07  initL(void)
  0.00     74.72     0.00        2     0.00     0.43  jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *)
  0.00     74.72     0.00        2     0.00     0.00  kMoraUseBucket(skStrategy *)
  0.00     74.72     0.00        2     0.00     0.00  mmInit(void)
  0.00     74.72     0.00        2     0.00     0.00  newFile(char *, _IO_FILE *)
  0.00     74.72     0.00        2     0.00     0.00  reinit_yylp(void)
  0.00     74.72     0.00        2     0.00     0.07  yylp_create_buffer(_IO_FILE *, int)
  0.00     74.72     0.00        2     0.00     0.00  yylprestart(_IO_FILE *)
  0.00     74.72     0.00        2     0.00     0.00  yylpwrap
  0.00     74.72     0.00        1     0.00     0.00  IsPrime(int)
  0.00     74.72     0.00        1     0.00     0.00  PrintLn
  0.00     74.72     0.00        1     0.00     0.00  SetProcs(p_Field, p_Length, p_Ord)
  0.00     74.72     0.00        1     0.00     0.00  global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     74.72     0.00        1     0.00     0.00  global destructors keyed to gmp_output_digits
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to MP_INT_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to sattr::Print(void)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to fglmVector::fglmVector(fglmVectorRep *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to idealFunctionals::idealFunctionals(int, int)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to convSingNClapN(snumber *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to dArith2
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to feOptSpec
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to feVersionId
  0.00     74.72     0.00        1     0.00     0.07  global constructors keyed to fe_promptstr
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to fglmUpdatesource(sip_sideal *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to fglmcomb.cc
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to gmp_output_digits
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to iiCurrArgs
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to ip_smatrix_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to jjSYSTEM(sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to kBucketCreate(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to mpsr_sleftv_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to omSingOutOfMemoryFunc
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to rnumber_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to sSubexpr_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to s_si_link_extension_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to singclap_gcd(spolyrec *, spolyrec *)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to sip_command_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to slists_bin
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &)
  0.00     74.72     0.00        1     0.00     0.00  global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &)
  0.00     74.72     0.00        1     0.00     0.00  _fe_getopt_internal
  0.00     74.72     0.00        1     0.00     0.00  _omIsOnList
  0.00     74.72     0.00        1     0.00     0.00  _omRemoveFromList
  0.00     74.72     0.00        1     0.00     0.88  _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  atGet(sleftv *, char *)
  0.00     74.72     0.00        1     0.00   192.02  exitBuchMora(skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  feGetExpandedExecutable(void)
  0.00     74.72     0.00        1     0.00     0.00  feInitResources(char *)
  0.00     74.72     0.00        1     0.00     0.00  feOptAction(feOptIndex)
  0.00     74.72     0.00        1     0.00     0.00  feSetOptValue(feOptIndex, int)
  0.00     74.72     0.00        1     0.00     0.00  fe_getopt_long
  0.00     74.72     0.00        1     0.00     0.00  full_readlink
  0.00     74.72     0.00        1     0.00     0.00  idHomIdeal(sip_sideal *, sip_sideal *)
  0.00     74.72     0.00        1     0.00     0.00  idSkipZeroes(sip_sideal *)
  0.00     74.72     0.00        1     0.00     0.01  iiExport(sleftv *, int)
  0.00     74.72     0.00        1     0.00     0.00  iiI2P(void *)
  0.00     74.72     0.00        1     0.00     0.00  initBuchMoraCrit(skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  initBuchMoraPos(skStrategy *)
  0.00     74.72     0.00        1     0.00    18.17  initBuchMora(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  initMora(sip_sideal *, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  initRTimer(void)
  0.00     74.72     0.00        1     0.00    18.03  initSL(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  initTimer(void)
  0.00     74.72     0.00        1     0.00     0.00  init_signals(void)
  0.00     74.72     0.00        1     0.00     0.00  inits(void)
  0.00     74.72     0.00        1     0.00     0.00  jiA_POLY(sleftv *, sleftv *, _ssubexpr *)
  0.00     74.72     0.00        1     0.00     0.00  jjInitTab1(void)
  0.00     74.72     0.00        1     0.00     0.00  jjJACOB_P(sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  jjOPTION_PL(sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  jjPLUS_I(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00 71520.50  jjSTD(sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  jjTIMES_I(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00 71520.50  kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *)
  0.00     74.72     0.00        1     0.00     0.00  m2_end
  0.00     74.72     0.00        1     0.00 71639.59  main
  0.00     74.72     0.00        1     0.00     0.00  messageStat(int, int, int, skStrategy *)
  0.00     74.72     0.00        1     0.00 71520.50  mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  mpsr_Init(void)
  0.00     74.72     0.00        1     0.00    20.14  nInitChar(sip_sring *)
  0.00     74.72     0.00        1     0.00    10.14  nSetChar(sip_sring *, short)
  0.00     74.72     0.00        1     0.00     0.00  npAdd(snumber *, snumber *)
  0.00     74.72     0.00        1     0.00     0.00  omFindExec
  0.00     74.72     0.00        1     0.00     0.00  omFindExec_link
  0.00     74.72     0.00        1     0.00     0.00  omFreeKeptAddrFromBin
  0.00     74.72     0.00        1     0.00     0.00  omGetStickyBinOfBin
  0.00     74.72     0.00        1     0.00     0.00  omInitInfo
  0.00     74.72     0.00        1     0.00     0.00  omIsKnownTopBin
  0.00     74.72     0.00        1     0.00     0.00  omMergeStickyBinIntoBin
  0.00     74.72     0.00        1     0.00     0.00  omSizeWOfAddr
  0.00     74.72     0.00        1     0.00     0.00  pIsHomogeneous(spolyrec *)
  0.00     74.72     0.00        1     0.00     0.00  pNormalize(spolyrec *)
  0.00     74.72     0.00        1     0.00     0.00  pSetGlobals(sip_sring *, short)
  0.00     74.72     0.00        1     0.00     0.00  p_FieldIs(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  p_GetSetmProc(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  p_OrdIs(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  p_SetProcs(sip_sring *, p_Procs_s *)
  0.00     74.72     0.00        1     0.00     0.00  pairs(skStrategy *)
  0.00     74.72     0.00        1     0.00    20.15  rComplete(sip_sring *, int)
  0.00     74.72     0.00        1     0.00     0.00  rFieldType(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  rGetDivMask(int)
  0.00     74.72     0.00        1     0.00     0.00  rGetExpSize(unsigned long, int &, int)
  0.00     74.72     0.00        1     0.00    20.17  rInit(sleftv *, sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  rOrd_is_Totaldegree_Ordering(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  rOrderName(char *)
  0.00     74.72     0.00        1     0.00     0.00  rRightAdjustVarOffset(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  rSetDegStuff(sip_sring *)
  0.00     74.72     0.00        1     0.00    10.14  rSetHdl(idrec *, short)
  0.00     74.72     0.00        1     0.00     0.00  rSetNegWeight(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  rSetVarL(sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  rSleftvList2StringArray(sleftv *, char **)
  0.00     74.72     0.00        1     0.00     0.00  rSleftvOrdering2Ordering(sleftv *, sip_sring *)
  0.00     74.72     0.00        1     0.00     0.00  reorderS(int *, skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  reorderT(skStrategy *)
  0.00     74.72     0.00        1     0.00     0.00  setOption(sleftv *, sleftv *)
  0.00     74.72     0.00        1     0.00     0.00  slInitDBMExtension(s_si_link_extension *)
  0.00     74.72     0.00        1     0.00     0.00  slInitMPFileExtension(s_si_link_extension *)
  0.00     74.72     0.00        1     0.00     0.00  slInitMPTcpExtension(s_si_link_extension *)
  0.00     74.72     0.00        1     0.00     0.00  slStandardInit(void)
  0.00     74.72     0.00        1     0.00     0.00  updateS(short, skStrategy *)
  0.00     74.72     0.00        1     0.00   416.40  updateT(skStrategy *)
  0.00     74.72     0.00        1     0.00     0.07  yy_create_buffer(_IO_FILE *, int)

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


granularity: each sample hit covers 4 byte(s) for 0.01% of 71.99 seconds

index % time    self  children    called     name
                0.00   71.64       1/1           _start [2]
[1]     99.5    0.00   71.64       1         main [1]
                0.02   71.61       1/1           yyparse(void) <cycle 1> [41]
                0.00    0.01       1/2           iiLibCmd(char *, short) [65]
                0.00    0.00       4/13          feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       2/2           newFile(char *, _IO_FILE *) [149]
                0.00    0.00       1/1           slStandardInit(void) [155]
                0.00    0.00       1/33          namerec::push(sip_package *, char *, int, short) [153]
                0.00    0.00       1/1           jjInitTab1(void) [340]
                0.00    0.00       1/1           inits(void) [338]
                0.00    0.00       1/1           feInitResources(char *) [325]
                0.00    0.00       1/1           fe_getopt_long [328]
                0.00    0.00       1/126         Print [199]
-----------------------------------------------
                                                 <spontaneous>
[2]     99.5    0.00   71.64                 _start [2]
                0.00   71.64       1/1           main [1]
-----------------------------------------------
[3]     99.5    0.02   71.61       1+466     <cycle 1 as a whole> [3]
                0.00   71.52      67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.02    0.08      33             yyparse(void) <cycle 1> [41]
                0.00    0.01     165             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                0.00    0.00      32             iiPStart(idrec *, sleftv *) <cycle 1> [98]
                0.00    0.00      88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
                0.00    0.00      32             iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
                0.00    0.00      18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [244]
                0.00    0.00      18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [243]
-----------------------------------------------
                                  67             yyparse(void) <cycle 1> [41]
[4]     99.3    0.00   71.52      67         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00   71.52       1/1           jjSTD(sleftv *, sleftv *) [5]
                0.00    0.00      90/965         sleftv::CleanUp(void) [75]
                0.00    0.00       1/1           jjJACOB_P(sleftv *, sleftv *) [158]
                0.00    0.00      67/1147        sleftv::Typ(void) [171]
                0.00    0.00      67/232         iiTabIndex(sValCmdTab const *, int, int) [190]
                0.00    0.00      23/76          iiTestConvert(int, int) [207]
                0.00    0.00      23/72          iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      18/18          jjCOUNT_L(sleftv *, sleftv *) [241]
                0.00    0.00      16/16          jjDEFINED(sleftv *, sleftv *) [254]
                0.00    0.00       7/563         sleftv::Data(void) [175]
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [270]
                0.00    0.00       3/3           jjMEMORY(sleftv *, sleftv *) [300]
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [256]
-----------------------------------------------
                0.00   71.52       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[5]     99.3    0.00   71.52       1         jjSTD(sleftv *, sleftv *) [5]
                0.00   71.52       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/1           idSkipZeroes(sip_sideal *) [156]
                0.00    0.00       1/1           atGet(sleftv *, char *) [323]
                0.00    0.00       1/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00   71.52       1/1           jjSTD(sleftv *, sleftv *) [5]
[6]     99.3    0.00   71.52       1         kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00   71.52       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       1/1           idHomIdeal(sip_sideal *, sip_sideal *) [159]
                0.00    0.00       1/64          __builtin_new [4308]
                0.00    0.00       1/16          sLObject::GetpLength(void) [253]
                0.00    0.00       1/1           omGetStickyBinOfBin [351]
                0.00    0.00       1/5           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [280]
                0.00    0.00       1/1           omMergeStickyBinIntoBin [354]
                0.00    0.00       1/63          __builtin_delete [4309]
-----------------------------------------------
                0.00   71.52       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[7]     99.3    0.00   71.52       1         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                2.55   48.37    3131/3131        redEcart(sLObject *, skStrategy *) [11]
                0.12   19.31    3161/3161        redFirst(sLObject *, skStrategy *) [13]
                0.00    0.58      63/63          enterSMora(sLObject, int, skStrategy *, int) [23]
                0.20    0.04      60/463         pNorm(spolyrec *) [15]
                0.00    0.19       1/1           exitBuchMora(skStrategy *) [36]
                0.00    0.10      63/470         enterT(sLObject, skStrategy *, int) [18]
                0.00    0.03      63/644         kBucketClear(kBucket *, spolyrec **, int *) [31]
                0.00    0.02     101/113         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.02       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
                0.00    0.00      63/63          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [97]
                0.00    0.00       2/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00      21/247738      omFreeToPageFault [30]
                0.00    0.00       3/60941       pLength(spolyrec *) [32]
                0.00    0.00      63/523         kBucketDestroy(kBucket **) [144]
                0.00    0.00       3/523         kBucketCreate(sip_sring *) [138]
                0.00    0.00    6292/6292        message(int, int *, int *, skStrategy *) [165]
                0.00    0.00      63/6304        PrintS [164]
                0.00    0.00      63/63          redtail(spolyrec *, int, skStrategy *) [216]
                0.00    0.00      63/533         cancelunit(sLObject *) [177]
                0.00    0.00      62/62          posInS(spolyrec **, int, spolyrec *) [217]
                0.00    0.00       3/16          sLObject::GetpLength(void) [253]
                0.00    0.00       3/697         kBucketInit(kBucket *, spolyrec *, int) [173]
                0.00    0.00       1/1           initBuchMoraCrit(skStrategy *) [331]
                0.00    0.00       1/1           initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [333]
                0.00    0.00       1/1           initMora(sip_sideal *, skStrategy *) [334]
                0.00    0.00       1/1           initBuchMoraPos(skStrategy *) [332]
                0.00    0.00       1/2           kMoraUseBucket(skStrategy *) [315]
                0.00    0.00       1/1           messageStat(int, int, int, skStrategy *) [345]
-----------------------------------------------
                0.11   18.86   99921/332946      redFirst(sLObject *, skStrategy *) [13]
                0.27   43.97  233025/332946      doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[8]     87.8    0.38   62.83  332946         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.16   62.09  332946/333047      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.47    0.02  332946/333047      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [26]
                0.04    0.00   24089/247738      omFreeToPageFault [30]
                0.03    0.00  332946/1258208     nDummy1(snumber **) [42]
                0.02    0.00  332946/1257408     npIsOne(snumber *) [43]
                0.00    0.00      53/523         kBucketDestroy(kBucket **) [144]
-----------------------------------------------
                0.00    0.02     101/333047      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.16   62.09  332946/333047      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[9]     86.5    0.16   62.11  333047         kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
               53.04    1.82  243348/243360      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
                6.29    0.25   64386/70422       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [14]
                0.27    0.17   34392/98945       pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.22    0.04   55307/189096      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.00    0.00  179398/179398      npNeg(snumber *) [163]
-----------------------------------------------
                0.00    0.00      12/243360      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
               53.04    1.82  243348/243360      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[10]    76.2   53.04    1.82  243360         p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
                0.52    0.32   64541/98945       pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.53    0.10  133685/189096      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.13    0.20  106573/243268      omAllocBinFromFullPage [20]
                0.02    0.00   13644/247738      omFreeToPageFault [30]
-----------------------------------------------
                2.55   48.37    3131/3131        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[11]    70.7    2.55   48.37    3131         redEcart(sLObject *, skStrategy *) [11]
                0.00   47.51  233025/233025      doRed(sLObject *, sTObject *, short, skStrategy *) [12]
                0.40    0.00  233355/333279      kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [25]
                0.18    0.00  236151/339251      p_GetShortExpVector(spolyrec *, sip_sring *) [34]
                0.17    0.00  175691/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
                0.06    0.00  233020/334636      pDeg(spolyrec *, sip_sring *) [44]
                0.00    0.03      60/1288        kBucketCanonicalize(kBucket *) [22]
                0.01    0.00    2796/5906        kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [48]
                0.00    0.00    3066/7047        enterL(sLObject **, int *, int *, sLObject, int) [131]
-----------------------------------------------
                0.00   47.51  233025/233025      redEcart(sLObject *, skStrategy *) [11]
[12]    66.0    0.00   47.51  233025         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
                0.27   43.97  233025/332946      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                1.30    0.24     400/463         pNorm(spolyrec *) [15]
                0.01    0.67     407/470         enterT(sLObject, skStrategy *, int) [18]
                0.58    0.08     814/818         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [21]
                0.00    0.20     407/644         kBucketClear(kBucket *, spolyrec **, int *) [31]
                0.00    0.19     407/1288        kBucketCanonicalize(kBucket *) [22]
                0.00    0.00     407/523         kBucketCreate(sip_sring *) [138]
                0.00    0.00     407/523         kBucketDestroy(kBucket **) [144]
                0.00    0.00     407/697         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.12   19.31    3161/3161        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[13]    27.0    0.12   19.31    3161         redFirst(sLObject *, skStrategy *) [13]
                0.11   18.86   99921/332946      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.17    0.00   99924/333279      kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [25]
                0.08    0.00  103034/339251      p_GetShortExpVector(spolyrec *, sip_sring *) [34]
                0.04    0.00   43687/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
                0.03    0.00   99876/334636      pDeg(spolyrec *, sip_sring *) [44]
                0.02    0.00    3110/5906        kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [48]
                0.00    0.00       3/1288        kBucketCanonicalize(kBucket *) [22]
                0.00    0.00       3/180         pLDeg0c(spolyrec *, int *, sip_sring *) [45]
                0.00    0.00    3110/7047        enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00       9/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00       6/70422       jjPLUS_P(sleftv *, sleftv *, sleftv *) [110]
                0.59    0.02    6030/70422       kBucketCanonicalize(kBucket *) [22]
                6.29    0.25   64386/70422       kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[14]     9.9    6.88    0.28   70422         p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [14]
                0.27    0.01  171765/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.01    0.00       3/463         initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.20    0.04      60/463         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                1.30    0.24     400/463         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[15]     2.5    1.51    0.28     463         pNorm(spolyrec *) [15]
                0.12    0.00  923651/923651      npDiv(snumber *, snumber *) [40]
                0.07    0.00  924114/1258208     nDummy1(snumber **) [42]
                0.07    0.00  924114/1257408     npIsOne(snumber *) [43]
                0.02    0.00  924082/924082      nDummy2(snumber *&) [57]
                0.00    0.00     463/602         npInit(int) [174]
-----------------------------------------------
                0.00    0.00      12/98945       ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.27    0.17   34392/98945       kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.52    0.32   64541/98945       p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
[16]     1.8    0.79    0.49   98945         pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.31    0.00   60938/60941       pLength(spolyrec *) [32]
                0.07    0.11   59262/243268      omAllocBinFromFullPage [20]
                0.00    0.00      10/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00       3/189096      _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00     101/189096      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.22    0.04   55307/189096      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.53    0.10  133685/189096      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
[17]     1.2    0.75    0.14  189096         pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.06    0.09   46447/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.10      63/470         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.01    0.67     407/470         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[18]     1.1    0.01    0.78     470         enterT(sLObject, skStrategy *, int) [18]
                0.73    0.05     470/470         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [19]
                0.00    0.00      59/62          omDoRealloc [96]
                0.00    0.00       3/243268      omAllocBinFromFullPage [20]
                0.00    0.00       2/247738      omFreeToPageFault [30]
                0.00    0.00     467/467         posInT17(sTObject *, int, sLObject const &) [179]
                0.00    0.00       3/3           posInT2(sTObject *, int, sLObject const &) [305]
-----------------------------------------------
                0.73    0.05     470/470         enterT(sLObject, skStrategy *, int) [18]
[19]     1.1    0.73    0.05     470         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [19]
                0.03    0.00   17509/247738      omFreeToPageFault [30]
                0.01    0.01    5526/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00       1/243268      idrec::set(char *, int, int, short) [152]
                0.00    0.00       1/243268      namerec::push(sip_package *, char *, int, short) [153]
                0.00    0.00       1/243268      iiCheckNest(void) [154]
                0.00    0.00       1/243268      enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
                0.00    0.00       1/243268      enterSBba(sLObject, int, skStrategy *, int) [145]
                0.00    0.00       1/243268      pEnlargeSet(spolyrec ***, int, int) [148]
                0.00    0.00       1/243268      rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00       1/243268      rComplete(sip_sring *, int) [54]
                0.00    0.00       1/243268      slStandardInit(void) [155]
                0.00    0.00       2/243268      hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [146]
                0.00    0.00       2/243268      iiGetLibProcBuffer(procinfo *, int) [139]
                0.00    0.00       2/243268      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00       3/243268      enterT(sLObject, skStrategy *, int) [18]
                0.00    0.00       5/243268      feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       5/243268      kBucketCreate(sip_sring *) [138]
                0.00    0.00       9/243268      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.00    0.00      18/243268      __builtin_vec_new [121]
                0.01    0.01    5526/243268      p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [19]
                0.03    0.05   25405/243268      p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [21]
                0.06    0.09   46447/243268      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.07    0.11   59262/243268      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.13    0.20  106573/243268      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
[20]     1.0    0.30    0.46  243268         omAllocBinFromFullPage [20]
                0.45    0.01    7524/7524        omAllocBinPage [27]
-----------------------------------------------
                0.00    0.00       1/818         pPower(spolyrec *, int) [94]
                0.00    0.00       3/818         initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.58    0.08     814/818         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[21]     0.9    0.58    0.08     818         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [21]
                0.03    0.05   25405/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00       3/1288        redFirst(sLObject *, skStrategy *) [13]
                0.00    0.03      60/1288        redEcart(sLObject *, skStrategy *) [11]
                0.00    0.08     174/1288        deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.19     407/1288        doRed(sLObject *, sTObject *, short, skStrategy *) [12]
                0.01    0.31     644/1288        kBucketClear(kBucket *, spolyrec **, int *) [31]
[22]     0.9    0.01    0.61    1288         kBucketCanonicalize(kBucket *) [22]
                0.59    0.02    6030/70422       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [14]
-----------------------------------------------
                0.00    0.58      63/63          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[23]     0.8    0.00    0.58      63         enterSMora(sLObject, int, skStrategy *, int) [23]
                0.00    0.42       4/4           firstUpdate(skStrategy *) [28]
                0.00    0.16       4/4           updateLHC(skStrategy *) [38]
                0.00    0.00       4/4           newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00       4/4           reorderL(skStrategy *) [116]
                0.00    0.00      63/63          enterSBba(sLObject, int, skStrategy *, int) [145]
                0.00    0.00      63/63          HEckeTest(spolyrec *, skStrategy *) [214]
-----------------------------------------------
                0.00    0.00       3/644         initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.07    0.08     174/644         updateLHC(skStrategy *) [38]
                0.20    0.22     467/644         updateT(skStrategy *) [29]
[24]     0.8    0.27    0.30     644         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.08     174/644         kBucketClear(kBucket *, spolyrec **, int *) [31]
                0.00    0.08     174/1288        kBucketCanonicalize(kBucket *) [22]
                0.08    0.00     174/180         pLDeg0c(spolyrec *, int *, sip_sring *) [45]
                0.05    0.00     135/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00     174/334636      pDeg(spolyrec *, sip_sring *) [44]
                0.00    0.00     174/177         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [195]
                0.00    0.00     174/697         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.17    0.00   99924/333279      redFirst(sLObject *, skStrategy *) [13]
                0.40    0.00  233355/333279      redEcart(sLObject *, skStrategy *) [11]
[25]     0.8    0.57    0.00  333279         kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [25]
-----------------------------------------------
                0.00    0.00     101/333047      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.47    0.02  332946/333047      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[26]     0.7    0.47    0.02  333047         p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [26]
                0.02    0.00    9530/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.45    0.01    7524/7524        omAllocBinFromFullPage [20]
[27]     0.6    0.45    0.01    7524         omAllocBinPage [27]
                0.00    0.01      57/57          omAllocNewBinPagesRegion [85]
-----------------------------------------------
                0.00    0.42       4/4           enterSMora(sLObject, int, skStrategy *, int) [23]
[28]     0.6    0.00    0.42       4         firstUpdate(skStrategy *) [28]
                0.00    0.42       1/1           updateT(skStrategy *) [29]
                0.00    0.00       1/2           kMoraUseBucket(skStrategy *) [315]
                0.00    0.00       1/1           reorderT(skStrategy *) [375]
-----------------------------------------------
                0.00    0.42       1/1           firstUpdate(skStrategy *) [28]
[29]     0.6    0.00    0.42       1         updateT(skStrategy *) [29]
                0.20    0.22     467/644         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.00     467/533         cancelunit(sLObject *) [177]
-----------------------------------------------
                0.00    0.00       1/247738      pEnlargeSet(spolyrec ***, int, int) [148]
                0.00    0.00       1/247738      rComplete(sip_sring *, int) [54]
                0.00    0.00       1/247738      omDoRealloc [96]
                0.00    0.00       2/247738      scComputeHC(sip_sideal *, int, spolyrec *&) [140]
                0.00    0.00       2/247738      hDelete(long **, int) [150]
                0.00    0.00       2/247738      updateLHC(skStrategy *) [38]
                0.00    0.00       2/247738      enterT(sLObject, skStrategy *, int) [18]
                0.00    0.00       3/247738      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.00    0.00       5/247738      feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       6/247738      kBucketDestroy(kBucket **) [144]
                0.00    0.00       9/247738      redFirst(sLObject *, skStrategy *) [13]
                0.00    0.00      10/247738      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.00    0.00      13/247738      deleteInL(sLObject *, int *, int, skStrategy *) [136]
                0.00    0.00      21/247738      mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      80/247738      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.02    0.00    9530/247738      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [26]
                0.02    0.00   11043/247738      p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.02    0.00   13644/247738      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
                0.03    0.00   17509/247738      p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [19]
                0.04    0.00   24089/247738      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.27    0.01  171765/247738      p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [14]
[30]     0.6    0.39    0.01  247738         omFreeToPageFault [30]
                0.01    0.00    5522/5522        omFreeBinPages [74]
-----------------------------------------------
                0.00    0.03      63/644         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.08     174/644         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.20     407/644         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[31]     0.4    0.00    0.31     644         kBucketClear(kBucket *, spolyrec **, int *) [31]
                0.01    0.31     644/1288        kBucketCanonicalize(kBucket *) [22]
-----------------------------------------------
                0.00    0.00       3/60941       mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.31    0.00   60938/60941       pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
[32]     0.4    0.31    0.00   60941         pLength(spolyrec *) [32]
-----------------------------------------------
                0.00    0.00       2/618         id_Delete(sip_sideal **, sip_sring *) [106]
                0.00    0.00       2/618         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       2/618         _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00       4/618         newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00       6/618         jjTIMES_P(sleftv *, sleftv *, sleftv *) [87]
                0.02    0.00      47/618         pmInit(char *, short &) [52]
                0.05    0.00     135/618         deleteHC(sLObject *, skStrategy *, short) [24]
                0.17    0.01     420/618         cleanT(skStrategy *) [37]
[33]     0.4    0.25    0.02     618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.02    0.00   11043/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00       3/339251      initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00      63/339251      enterpairs(spolyrec *, int, int, int, skStrategy *, int) [97]
                0.08    0.00  103034/339251      redFirst(sLObject *, skStrategy *) [13]
                0.18    0.00  236151/339251      redEcart(sLObject *, skStrategy *) [11]
[34]     0.4    0.26    0.00  339251         p_GetShortExpVector(spolyrec *, sip_sring *) [34]
-----------------------------------------------
                0.00    0.00       2/220430      initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00     182/220430      reorderL(skStrategy *) [116]
                0.00    0.00     192/220430      chainCrit(spolyrec *, int, skStrategy *) [115]
                0.00    0.00     676/220430      enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
                0.04    0.00   43687/220430      redFirst(sLObject *, skStrategy *) [13]
                0.17    0.00  175691/220430      redEcart(sLObject *, skStrategy *) [11]
[35]     0.3    0.21    0.00  220430         posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
-----------------------------------------------
                0.00    0.19       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[36]     0.3    0.00    0.19       1         exitBuchMora(skStrategy *) [36]
                0.01    0.18       1/1           cleanT(skStrategy *) [37]
                0.00    0.00       1/1258208     nDummy1(snumber **) [42]
                0.00    0.00       5/186         omSizeOfLargeAddr [194]
                0.00    0.00       5/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.01    0.18       1/1           exitBuchMora(skStrategy *) [36]
[37]     0.3    0.01    0.18       1         cleanT(skStrategy *) [37]
                0.17    0.01     420/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
-----------------------------------------------
                0.00    0.16       4/4           enterSMora(sLObject, int, skStrategy *, int) [23]
[38]     0.2    0.00    0.16       4         updateLHC(skStrategy *) [38]
                0.07    0.08     174/644         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.00      12/113         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00      24/334636      pDeg(spolyrec *, sip_sring *) [44]
                0.00    0.00       2/247738      omFreeToPageFault [30]
                0.00    0.00      12/523         kBucketCreate(sip_sring *) [138]
                0.00    0.00       4/563         deleteInL(sLObject *, int *, int, skStrategy *) [136]
                0.00    0.00      12/16          sLObject::GetpLength(void) [253]
                0.00    0.00      12/697         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                                                 <spontaneous>
[39]     0.2    0.14    0.00                 write [39]
-----------------------------------------------
                0.12    0.00  923651/923651      pNorm(spolyrec *) [15]
[40]     0.2    0.12    0.00  923651         npDiv(snumber *, snumber *) [40]
-----------------------------------------------
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [98]
                0.02   71.61       1/1           main [1]
[41]     0.1    0.02    0.08      33         yyparse(void) <cycle 1> [41]
                0.00    0.02     320/320         syMake(sleftv *, char *, idrec *) [51]
                0.00    0.02       1/1           rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.01    1788/1788        yylex(MYYSTYPE *) [64]
                0.00    0.01       1/1           rSetHdl(idrec *, short) [70]
                0.00    0.01      64/103         iiAssign(sleftv *, sleftv *) [62]
                0.00    0.01       1/2           iiLibCmd(char *, short) [65]
                0.00    0.00      45/45          iiParameter(sleftv *) [86]
                0.00    0.00      28/60          newBuffer(char *, feBufferTypes, procinfo *, int) [91]
                0.00    0.00      77/77          iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [109]
                0.00    0.00      63/965         sleftv::CleanUp(void) [75]
                0.00    0.00      26/26          sleftv::Print(sleftv *, int) [112]
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [113]
                0.00    0.00       1/1           iiExport(sleftv *, int) [143]
                0.00    0.00       1/120         enterid(char *, int, int, idrec **, short) [151]
                0.00    0.00      76/1147        sleftv::Typ(void) [171]
                0.00    0.00      51/563         sleftv::Data(void) [175]
                0.00    0.00      32/32          exitBuffer(feBufferTypes) [228]
                0.00    0.00      20/32          sleftv::Copy(sleftv *) [227]
                0.00    0.00       1/1           rOrderName(char *) [367]
                0.00    0.00       1/64          __builtin_new [4308]
                                 147             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                                  88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
                                  67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
-----------------------------------------------
                0.00    0.00       1/1258208     exitBuchMora(skStrategy *) [36]
                0.00    0.00       2/1258208     _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00      14/1258208     pDiff(spolyrec *, int) [157]
                0.00    0.00     113/1258208     ksCheckCoeff(snumber **, snumber **) [134]
                0.00    0.00     226/1258208     ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00     792/1258208     ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.03    0.00  332946/1258208     ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.07    0.00  924114/1258208     pNorm(spolyrec *) [15]
[42]     0.1    0.10    0.00 1258208         nDummy1(snumber **) [42]
-----------------------------------------------
                0.00    0.00       9/1257408     pPower(spolyrec *, int) [94]
                0.00    0.00     339/1257408     ksCheckCoeff(snumber **, snumber **) [134]
                0.02    0.00  332946/1257408     ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.07    0.00  924114/1257408     pNorm(spolyrec *) [15]
[43]     0.1    0.09    0.00 1257408         npIsOne(snumber *) [43]
-----------------------------------------------
                0.00    0.00       3/334636      initEcartNormal(sLObject *) [99]
                0.00    0.00       3/334636      pIsHomogeneous(spolyrec *) [160]
                0.00    0.00       4/334636      newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00      24/334636      updateLHC(skStrategy *) [38]
                0.00    0.00     174/334636      deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.00     180/334636      pLDeg0c(spolyrec *, int *, sip_sring *) [45]
                0.00    0.00    1352/334636      initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.03    0.00   99876/334636      redFirst(sLObject *, skStrategy *) [13]
                0.06    0.00  233020/334636      redEcart(sLObject *, skStrategy *) [11]
[44]     0.1    0.09    0.00  334636         pDeg(spolyrec *, sip_sring *) [44]
-----------------------------------------------
                0.00    0.00       3/180         redFirst(sLObject *, skStrategy *) [13]
                0.00    0.00       3/180         initEcartNormal(sLObject *) [99]
                0.08    0.00     174/180         deleteHC(sLObject *, skStrategy *, short) [24]
[45]     0.1    0.08    0.00     180         pLDeg0c(spolyrec *, int *, sip_sring *) [45]
                0.00    0.00     180/334636      pDeg(spolyrec *, sip_sring *) [44]
-----------------------------------------------
                                                 <spontaneous>
[46]     0.1    0.04    0.00                 fflush [46]
-----------------------------------------------
                                                 <spontaneous>
[47]     0.1    0.04    0.00                 fork [47]
-----------------------------------------------
                0.01    0.00    2796/5906        redEcart(sLObject *, skStrategy *) [11]
                0.02    0.00    3110/5906        redFirst(sLObject *, skStrategy *) [13]
[48]     0.0    0.03    0.00    5906         kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [48]
-----------------------------------------------
                                                 <spontaneous>
[49]     0.0    0.03    0.00                 _IO_file_write [49]
-----------------------------------------------
                0.00    0.00      12/113         updateLHC(skStrategy *) [38]
                0.00    0.02     101/113         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[50]     0.0    0.00    0.02     113         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.02     101/333047      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.00    0.00      12/243360      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [10]
                0.00    0.00     101/189096      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.00    0.00      12/98945       pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [16]
                0.00    0.00     101/333047      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [26]
                0.00    0.00      80/247738      omFreeToPageFault [30]
                0.00    0.00     113/113         ksCheckCoeff(snumber **, snumber **) [134]
                0.00    0.00     226/1258208     nDummy1(snumber **) [42]
                0.00    0.00       2/243268      omAllocBinFromFullPage [20]
                0.00    0.00     101/523         kBucketCreate(sip_sring *) [138]
                0.00    0.00     226/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
                0.00    0.00     101/697         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.02     320/320         yyparse(void) <cycle 1> [41]
[51]     0.0    0.00    0.02     320         syMake(sleftv *, char *, idrec *) [51]
                0.00    0.02      49/49          pmInit(char *, short &) [52]
                0.00    0.00     320/320         ggetid(char const *, short) [189]
                0.00    0.00      19/117         rIsRingVar(char *) [201]
                0.00    0.00      10/11          p_ISet(int, sip_sring *) [259]
                0.00    0.00      10/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
                0.00    0.00       2/3           pIsConstant(spolyrec *) [304]
-----------------------------------------------
                0.00    0.02      49/49          syMake(sleftv *, char *, idrec *) [51]
[52]     0.0    0.00    0.02      49         pmInit(char *, short &) [52]
                0.02    0.00      47/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00      98/117         rIsRingVar(char *) [201]
                0.00    0.00      49/49          npRead(char *, snumber **) [221]
                0.00    0.00       2/2           eati(char *, int *) [308]
                0.00    0.00       2/28          npIsZero(snumber *) [234]
                0.00    0.00       2/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
-----------------------------------------------
                0.00    0.02       1/1           yyparse(void) <cycle 1> [41]
[53]     0.0    0.00    0.02       1         rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.02       1/1           rComplete(sip_sring *, int) [54]
                0.00    0.00       2/965         sleftv::CleanUp(void) [75]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00       1/1147        sleftv::Typ(void) [171]
                0.00    0.00       1/563         sleftv::Data(void) [175]
                0.00    0.00       1/1           IsPrime(int) [320]
                0.00    0.00       1/340         sleftv::listLength(void) [188]
                0.00    0.00       1/1           rSleftvList2StringArray(sleftv *, char **) [372]
                0.00    0.00       1/1           rSleftvOrdering2Ordering(sleftv *, sip_sring *) [373]
                0.00    0.00       1/3           rDBTest(sip_sring *, char *, int) [306]
-----------------------------------------------
                0.00    0.02       1/1           rInit(sleftv *, sleftv *, sleftv *) [53]
[54]     0.0    0.00    0.02       1         rComplete(sip_sring *, int) [54]
                0.00    0.02       1/1           nInitChar(sip_sring *) [56]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00       1/247738      omFreeToPageFault [30]
                0.00    0.00       1/1           omSizeWOfAddr [355]
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [369]
                0.00    0.00       1/1           rGetExpSize(unsigned long, int &, int) [365]
                0.00    0.00       1/1           rGetDivMask(int) [364]
                0.00    0.00       1/42          _omGetSpecBin [4311]
                0.00    0.00       1/1           p_GetSetmProc(sip_sring *) [359]
                0.00    0.00       1/1           rSetVarL(sip_sring *) [371]
                0.00    0.00       1/1           rRightAdjustVarOffset(sip_sring *) [368]
                0.00    0.00       1/1           rSetNegWeight(sip_sring *) [370]
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [361]
-----------------------------------------------
                0.02    0.00       1/1           nInitChar(sip_sring *) [56]
[55]     0.0    0.02    0.00       1         npInitChar(int, sip_sring *) [55]
                0.00    0.00       2/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.02       1/1           rComplete(sip_sring *, int) [54]
[56]     0.0    0.00    0.02       1         nInitChar(sip_sring *) [56]
                0.02    0.00       1/1           npInitChar(int, sip_sring *) [55]
                0.00    0.00       1/1           rFieldType(sip_sring *) [363]
                0.00    0.00       1/602         npInit(int) [174]
-----------------------------------------------
                0.02    0.00  924082/924082      pNorm(spolyrec *) [15]
[57]     0.0    0.02    0.00  924082         nDummy2(snumber *&) [57]
-----------------------------------------------
                                                 <spontaneous>
[58]     0.0    0.02    0.00                 memmove [58]
-----------------------------------------------
                                                 <spontaneous>
[59]     0.0    0.02    0.00                 _IO_file_xsputn [59]
-----------------------------------------------
                0.00    0.02       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[60]     0.0    0.00    0.02       1         initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
                0.00    0.02       1/1           initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00       2/2           initL(void) [119]
                0.00    0.00       1/1           updateS(short, skStrategy *) [380]
                0.00    0.00       1/1           pairs(skStrategy *) [362]
-----------------------------------------------
                0.00    0.02       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
[61]     0.0    0.00    0.02       1         initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.01    0.00       3/463         pNorm(spolyrec *) [15]
                0.00    0.00       3/644         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.00       3/818         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [21]
                0.00    0.00       3/3           initEcartNormal(sLObject *) [99]
                0.00    0.00       3/339251      p_GetShortExpVector(spolyrec *, sip_sring *) [34]
                0.00    0.00       2/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
                0.00    0.00       3/7047        enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00       3/533         cancelunit(sLObject *) [177]
                0.00    0.00       1/4           idInit(int, int) [290]
                0.00    0.00       1/3           pIsConstant(spolyrec *) [304]
-----------------------------------------------
                0.00    0.00      39/103         iiParameter(sleftv *) [86]
                0.00    0.01      64/103         yyparse(void) <cycle 1> [41]
[62]     0.0    0.00    0.01     103         iiAssign(sleftv *, sleftv *) [62]
                0.00    0.01      91/91          jiAssign_1(sleftv *, sleftv *) [63]
                0.00    0.00     194/965         sleftv::CleanUp(void) [75]
                0.00    0.00      12/12          jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00     206/340         sleftv::listLength(void) [188]
                0.00    0.00     206/1147        sleftv::Typ(void) [171]
                0.00    0.00      20/32          ipMoveId(idrec *) [230]
-----------------------------------------------
                0.00    0.01      91/91          iiAssign(sleftv *, sleftv *) [62]
[63]     0.0    0.00    0.01      91         jiAssign_1(sleftv *, sleftv *) [63]
                0.01    0.00      65/65          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [76]
                0.00    0.00       2/2           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [107]
                0.00    0.00     182/1147        sleftv::Typ(void) [171]
                0.00    0.00      23/23          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [237]
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [339]
-----------------------------------------------
                0.00    0.01    1788/1788        yyparse(void) <cycle 1> [41]
[64]     0.0    0.00    0.01    1788         yylex(MYYSTYPE *) [64]
                0.00    0.01     782/782         yy_get_next_buffer(void) [68]
                0.00    0.00       1/1           yy_create_buffer(_IO_FILE *, int) [128]
                0.00    0.00     766/766         yy_get_previous_state(void) [172]
                0.00    0.00     502/502         IsCmd(char *, int &) [178]
                0.00    0.00      16/61          exitVoice(void) [218]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [233]
                0.00    0.00       1/97          yy_load_buffer_state(void) [204]
                0.00    0.00       1/1           m2_end [344]
-----------------------------------------------
                0.00    0.01       1/2           main [1]
                0.00    0.01       1/2           yyparse(void) <cycle 1> [41]
[65]     0.0    0.00    0.01       2         iiLibCmd(char *, short) [65]
                0.00    0.01       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [66]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       1/120         enterid(char *, int, int, idrec **, short) [151]
                0.00    0.00       2/4           idrec::get(char const *, int) [283]
-----------------------------------------------
                0.00    0.01       2/2           iiLibCmd(char *, short) [65]
[66]     0.0    0.00    0.01       2         iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [66]
                0.01    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
                0.00    0.00       2/2           reinit_yylp(void) [317]
                0.00    0.00       1/126         Print [199]
-----------------------------------------------
                0.01    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [66]
[67]     0.0    0.01    0.00       2         yylplex(char *, char *, lib_style_types *, lp_modes) [67]
                0.00    0.00       2/2           yylp_create_buffer(_IO_FILE *, int) [120]
                0.00    0.00      41/120         enterid(char *, int, int, idrec **, short) [151]
                0.00    0.00     403/403         current_pos(int) [181]
                0.00    0.00     347/347         copy_string(lp_modes) [187]
                0.00    0.00      41/41          iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [223]
                0.00    0.00       9/9           yy_get_next_buffer(void) [265]
                0.00    0.00       7/7           yy_get_previous_state(void) [274]
                0.00    0.00       4/4           make_version(char *, int) [293]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [298]
                0.00    0.00       2/2           yylpwrap [319]
-----------------------------------------------
                0.00    0.01     782/782         yylex(MYYSTYPE *) [64]
[68]     0.0    0.00    0.01     782         yy_get_next_buffer(void) [68]
                0.01    0.00     782/782         feReadLine(char *, int) [71]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [233]
-----------------------------------------------
                0.00    0.01       1/1           rSetHdl(idrec *, short) [70]
[69]     0.0    0.00    0.01       1         nSetChar(sip_sring *, short) [69]
                0.01    0.00       1/1           npSetChar(int, sip_sring *) [72]
-----------------------------------------------
                0.00    0.01       1/1           yyparse(void) <cycle 1> [41]
[70]     0.0    0.00    0.01       1         rSetHdl(idrec *, short) [70]
                0.00    0.01       1/1           nSetChar(sip_sring *, short) [69]
                0.00    0.00       2/3           rDBTest(sip_sring *, char *, int) [306]
                0.00    0.00       1/1           pSetGlobals(sip_sring *, short) [357]
-----------------------------------------------
                0.01    0.00     782/782         yy_get_next_buffer(void) [68]
[71]     0.0    0.01    0.00     782         feReadLine(char *, int) [71]
                0.00    0.00       2/109         omAllocFromSystem [84]
                0.00    0.00     375/375         fePrintEcho(char *, char *) [182]
-----------------------------------------------
                0.01    0.00       1/1           nSetChar(sip_sring *, short) [69]
[72]     0.0    0.01    0.00       1         npSetChar(int, sip_sring *) [72]
                0.00    0.00       2/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00      32/141         rEALLOc [90]
                0.01    0.00     109/141         omAllocFromSystem [84]
[73]     0.0    0.01    0.00     141         mALLOc [73]
                0.00    0.00      11/11          malloc_extend_top [258]
-----------------------------------------------
                0.01    0.00    5522/5522        omFreeToPageFault [30]
[74]     0.0    0.01    0.00    5522         omFreeBinPages [74]
                0.00    0.00      18/18          omFreeBinPagesRegion [245]
-----------------------------------------------
                                  47             sleftv::CleanUp(void) [75]
                0.00    0.00       1/965         iiExport(sleftv *, int) [143]
                0.00    0.00       2/965         rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00      11/965         iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      12/965         jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      12/965         killhdl(idrec *, idrec **) [123]
                0.00    0.00      21/965         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [113]
                0.00    0.00      26/965         sleftv::Print(sleftv *, int) [112]
                0.00    0.00      63/965         yyparse(void) <cycle 1> [41]
                0.00    0.00      77/965         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [109]
                0.00    0.00      88/965         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
                0.00    0.00      90/965         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     194/965         iiAssign(sleftv *, sleftv *) [62]
                0.00    0.00     368/965         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[75]     0.0    0.01    0.00     965+47      sleftv::CleanUp(void) [75]
                0.00    0.00      18/18          slKill(sip_link *) [251]
                0.00    0.00       1/63          __builtin_delete [4309]
                                  47             sleftv::CleanUp(void) [75]
-----------------------------------------------
                0.01    0.00      65/65          jiAssign_1(sleftv *, sleftv *) [63]
[76]     0.0    0.01    0.00      65         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [76]
                0.00    0.00      65/122         sleftv::CopyD(int) [200]
                0.00    0.00      65/91          sleftv::LData(void) [205]
-----------------------------------------------
                                                 <spontaneous>
[77]     0.0    0.01    0.00                 flockfile [77]
-----------------------------------------------
                                                 <spontaneous>
[78]     0.0    0.01    0.00                 fwrite [78]
-----------------------------------------------
                                                 <spontaneous>
[79]     0.0    0.01    0.00                 memset [79]
-----------------------------------------------
                                                 <spontaneous>
[80]     0.0    0.01    0.00                 strcat [80]
-----------------------------------------------
                                                 <spontaneous>
[81]     0.0    0.01    0.00                 system [81]
-----------------------------------------------
                                                 <spontaneous>
[82]     0.0    0.01    0.00                 _IO_do_write [82]
-----------------------------------------------
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [243]
                                 147             yyparse(void) <cycle 1> [41]
[83]     0.0    0.00    0.01     165         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                0.00    0.00     368/965         sleftv::CleanUp(void) [75]
                0.00    0.00       6/6           jjTIMES_P(sleftv *, sleftv *, sleftv *) [87]
                0.00    0.00      10/10          jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
                0.00    0.00       6/6           jjPLUS_P(sleftv *, sleftv *, sleftv *) [110]
                0.00    0.00     330/1147        sleftv::Typ(void) [171]
                0.00    0.00     165/232         iiTabIndex(sValCmdTab const *, int, int) [190]
                0.00    0.00      90/90          jjPLUS_S(sleftv *, sleftv *, sleftv *) [206]
                0.00    0.00      42/76          iiTestConvert(int, int) [207]
                0.00    0.00      38/72          iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      18/18          jjGT_I(sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      11/11          jjMINUS_I(sleftv *, sleftv *, sleftv *) [257]
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [291]
                0.00    0.00       1/1           jjPLUS_I(sleftv *, sleftv *, sleftv *) [342]
                0.00    0.00       1/1           jjTIMES_I(sleftv *, sleftv *, sleftv *) [343]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [244]
-----------------------------------------------
                0.00    0.00       1/109         global constructors keyed to fe_promptstr [129]
                0.00    0.00       1/109         __builtin_vec_new [121]
                0.00    0.00       1/109         omBinPageIndexFault [101]
                0.00    0.00       2/109         feReadLine(char *, int) [71]
                0.00    0.00       2/109         initL(void) [119]
                0.00    0.00       2/109         npSetChar(int, sip_sring *) [72]
                0.00    0.00       2/109         npInitChar(int, sip_sring *) [55]
                0.00    0.00       2/109         yy_flex_alloc(unsigned int) [118]
                0.00    0.00       3/109         malloc [114]
                0.00    0.00       3/109         omDoRealloc [96]
                0.00    0.00      33/109         yy_flex_alloc(unsigned int) [88]
                0.00    0.00      57/109         omAllocNewBinPagesRegion [85]
[84]     0.0    0.00    0.01     109         omAllocFromSystem [84]
                0.01    0.00     109/141         mALLOc [73]
-----------------------------------------------
                0.00    0.01      57/57          omAllocBinPage [27]
[85]     0.0    0.00    0.01      57         omAllocNewBinPagesRegion [85]
                0.00    0.00      57/109         omAllocFromSystem [84]
                0.00    0.00      57/57          omRegisterBinPages [102]
                0.00    0.00      57/57          _omVallocFromSystem [4310]
-----------------------------------------------
                0.00    0.00      45/45          yyparse(void) <cycle 1> [41]
[86]     0.0    0.00    0.00      45         iiParameter(sleftv *) [86]
                0.00    0.00      39/103         iiAssign(sleftv *, sleftv *) [62]
-----------------------------------------------
                0.00    0.00       6/6           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[87]     0.0    0.00    0.00       6         jjTIMES_P(sleftv *, sleftv *, sleftv *) [87]
                0.00    0.00       6/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00      12/122         sleftv::CopyD(int) [200]
                0.00    0.00       6/6           p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [279]
-----------------------------------------------
                0.00    0.00       2/66          yy_create_buffer(_IO_FILE *, int) [128]
                0.00    0.00      64/66          myynewbuffer(void) [92]
[88]     0.0    0.00    0.00      66         yy_flex_alloc(unsigned int) [88]
                0.00    0.00      33/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00      17/115         omDoRealloc [96]
                0.00    0.00      42/115         omRealloc0Large [108]
                0.00    0.00      56/115         omBinPageIndexFault [101]
[89]     0.0    0.00    0.00     115         omReallocSizeFromSystem [89]
                0.00    0.00     115/115         rEALLOc [90]
-----------------------------------------------
                0.00    0.00     115/115         omReallocSizeFromSystem [89]
[90]     0.0    0.00    0.00     115         rEALLOc [90]
                0.00    0.00      32/141         mALLOc [73]
                0.00    0.00      93/154         fREe [197]
-----------------------------------------------
                0.00    0.00      28/60          yyparse(void) <cycle 1> [41]
                0.00    0.00      32/60          iiPStart(idrec *, sleftv *) <cycle 1> [98]
[91]     0.0    0.00    0.00      60         newBuffer(char *, feBufferTypes, procinfo *, int) [91]
                0.00    0.00      32/32          myynewbuffer(void) [92]
                0.00    0.00      60/64          __builtin_new [4308]
-----------------------------------------------
                0.00    0.00      32/32          newBuffer(char *, feBufferTypes, procinfo *, int) [91]
[92]     0.0    0.00    0.00      32         myynewbuffer(void) [92]
                0.00    0.00      64/66          yy_flex_alloc(unsigned int) [88]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [212]
                0.00    0.00      32/97          yy_load_buffer_state(void) [204]
-----------------------------------------------
                0.00    0.00       4/4           enterSMora(sLObject, int, skStrategy *, int) [23]
[93]     0.0    0.00    0.00       4         newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00       4/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
                0.00    0.00       4/334636      pDeg(spolyrec *, sip_sring *) [44]
                0.00    0.00       4/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
                0.00    0.00       4/126         Print [199]
                0.00    0.00       3/177         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [195]
-----------------------------------------------
                0.00    0.00      10/10          jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
[94]     0.0    0.00    0.00      10         pPower(spolyrec *, int) [94]
                0.00    0.00       1/1           _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00       1/818         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [21]
                0.00    0.00       9/1257408     npIsOne(snumber *) [43]
                0.00    0.00       9/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
-----------------------------------------------
                0.00    0.00      10/10          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[95]     0.0    0.00    0.00      10         jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
                0.00    0.00      10/10          pPower(spolyrec *, int) [94]
                0.00    0.00      10/563         sleftv::Data(void) [175]
                0.00    0.00      10/122         sleftv::CopyD(int) [200]
                0.00    0.00      10/10          jjOP_REST(sleftv *, sleftv *, sleftv *) [262]
-----------------------------------------------
                0.00    0.00       3/62          enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00      59/62          enterT(sLObject, skStrategy *, int) [18]
[96]     0.0    0.00    0.00      62         omDoRealloc [96]
                0.00    0.00      42/42          omRealloc0Large [108]
                0.00    0.00      17/115         omReallocSizeFromSystem [89]
                0.00    0.00       3/109         omAllocFromSystem [84]
                0.00    0.00       1/247738      omFreeToPageFault [30]
                0.00    0.00      17/186         omSizeOfLargeAddr [194]
                0.00    0.00       6/6           omSizeOfAddr [278]
-----------------------------------------------
                0.00    0.00      63/63          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[97]     0.0    0.00    0.00      63         enterpairs(spolyrec *, int, int, int, skStrategy *, int) [97]
                0.00    0.00      63/63          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
                0.00    0.00      63/339251      p_GetShortExpVector(spolyrec *, sip_sring *) [34]
-----------------------------------------------
                                  32             iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
[98]     0.0    0.00    0.00      32         iiPStart(idrec *, sleftv *) <cycle 1> [98]
                0.00    0.00      32/60          newBuffer(char *, feBufferTypes, procinfo *, int) [91]
                0.00    0.00      32/32          killlocals(int) [124]
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [139]
                                  32             yyparse(void) <cycle 1> [41]
-----------------------------------------------
                0.00    0.00       3/3           initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
[99]     0.0    0.00    0.00       3         initEcartNormal(sLObject *) [99]
                0.00    0.00       3/180         pLDeg0c(spolyrec *, int *, sip_sring *) [45]
                0.00    0.00       3/334636      pDeg(spolyrec *, sip_sring *) [44]
-----------------------------------------------
                0.00    0.00      63/63          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [97]
[100]    0.0    0.00    0.00      63         initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
                0.00    0.00    1587/1587        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
                0.00    0.00      63/63          chainCrit(spolyrec *, int, skStrategy *) [115]
-----------------------------------------------
                0.00    0.00      57/57          omRegisterBinPages [102]
[101]    0.0    0.00    0.00      57         omBinPageIndexFault [101]
                0.00    0.00      56/115         omReallocSizeFromSystem [89]
                0.00    0.00       1/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00      57/57          omAllocNewBinPagesRegion [85]
[102]    0.0    0.00    0.00      57         omRegisterBinPages [102]
                0.00    0.00      57/57          omBinPageIndexFault [101]
-----------------------------------------------
                0.00    0.00    1587/1587        initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
[103]    0.0    0.00    0.00    1587         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
                0.00    0.00     676/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
                0.00    0.00     676/676         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.00    0.00     676/676         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.00    0.00     484/563         deleteInL(sLObject *, int *, int, skStrategy *) [136]
                0.00    0.00     676/7047        enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00    1587/1587        pLcm(spolyrec *, spolyrec *, spolyrec *) [168]
                0.00    0.00    1587/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
                0.00    0.00    1587/1587        pHasNotCF(spolyrec *, spolyrec *) [167]
-----------------------------------------------
                                  88             yyparse(void) <cycle 1> [41]
[104]    0.0    0.00    0.00      88         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
                0.00    0.00      88/965         sleftv::CleanUp(void) [75]
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      88/340         sleftv::listLength(void) [188]
                0.00    0.00      33/33          jjSTRING_PL(sleftv *, sleftv *) [226]
                0.00    0.00      25/25          jjSYSTEM(sleftv *, sleftv *) [236]
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [341]
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [243]
-----------------------------------------------
                0.00    0.00       1/1           pPower(spolyrec *, int) [94]
[105]    0.0    0.00    0.00       1         _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00       2/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
                0.00    0.00       3/189096      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [17]
                0.00    0.00       2/1258208     nDummy1(snumber **) [42]
                0.00    0.00       1/1           npAdd(snumber *, snumber *) [347]
                0.00    0.00       1/28          npIsZero(snumber *) [234]
-----------------------------------------------
                0.00    0.00       2/2           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [107]
[106]    0.0    0.00    0.00       2         id_Delete(sip_sideal **, sip_sring *) [106]
                0.00    0.00       2/618         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [33]
-----------------------------------------------
                0.00    0.00       2/2           jiAssign_1(sleftv *, sleftv *) [63]
[107]    0.0    0.00    0.00       2         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [107]
                0.00    0.00       2/2           id_Delete(sip_sideal **, sip_sring *) [106]
                0.00    0.00       2/122         sleftv::CopyD(int) [200]
                0.00    0.00       2/91          sleftv::LData(void) [205]
-----------------------------------------------
                0.00    0.00      42/42          omDoRealloc [96]
[108]    0.0    0.00    0.00      42         omRealloc0Large [108]
                0.00    0.00      42/115         omReallocSizeFromSystem [89]
                0.00    0.00     126/186         omSizeOfLargeAddr [194]
-----------------------------------------------
                0.00    0.00      77/77          yyparse(void) <cycle 1> [41]
[109]    0.0    0.00    0.00      77         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [109]
                0.00    0.00      77/965         sleftv::CleanUp(void) [75]
                0.00    0.00      77/120         enterid(char *, int, int, idrec **, short) [151]
-----------------------------------------------
                0.00    0.00       6/6           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[110]    0.0    0.00    0.00       6         jjPLUS_P(sleftv *, sleftv *, sleftv *) [110]
                0.00    0.00       6/70422       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [14]
                0.00    0.00      12/122         sleftv::CopyD(int) [200]
                0.00    0.00       6/108         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [203]
-----------------------------------------------
                0.00    0.00     676/676         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[111]    0.0    0.00    0.00     676         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [111]
                0.00    0.00    1352/334636      pDeg(spolyrec *, sip_sring *) [44]
-----------------------------------------------
                0.00    0.00      26/26          yyparse(void) <cycle 1> [41]
[112]    0.0    0.00    0.00      26         sleftv::Print(sleftv *, int) [112]
                0.00    0.00      26/965         sleftv::CleanUp(void) [75]
                0.00    0.00      27/1147        sleftv::Typ(void) [171]
                0.00    0.00      26/563         sleftv::Data(void) [175]
                0.00    0.00       1/126         Print [199]
                0.00    0.00       1/1           PrintLn [321]
                0.00    0.00       1/122         sleftv::CopyD(int) [200]
-----------------------------------------------
                0.00    0.00       7/7           yyparse(void) <cycle 1> [41]
[113]    0.0    0.00    0.00       7         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [113]
                0.00    0.00      21/965         sleftv::CleanUp(void) [75]
                0.00    0.00      21/1147        sleftv::Typ(void) [171]
                0.00    0.00       7/7           jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [269]
-----------------------------------------------
                0.00    0.00       2/36          __gmpf_init_set_d [141]
                0.00    0.00       2/36          __gmpz_init_set_si [142]
                0.00    0.00       3/36          opendir [137]
                0.00    0.00      29/36          fopen [117]
[114]    0.0    0.00    0.00      36         malloc [114]
                0.00    0.00       3/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00      63/63          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
[115]    0.0    0.00    0.00      63         chainCrit(spolyrec *, int, skStrategy *) [115]
                0.00    0.00     192/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
                0.00    0.00      75/563         deleteInL(sLObject *, int *, int, skStrategy *) [136]
                0.00    0.00     192/7047        enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00    1297/1297        pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [170]
-----------------------------------------------
                0.00    0.00       4/4           enterSMora(sLObject, int, skStrategy *, int) [23]
[116]    0.0    0.00    0.00       4         reorderL(skStrategy *) [116]
                0.00    0.00     182/220430      posInL17(sLObject *, int, sLObject *, skStrategy *) [35]
-----------------------------------------------
                                                 <spontaneous>
[117]    0.0    0.00    0.00                 fopen [117]
                0.00    0.00      29/36          malloc [114]
-----------------------------------------------
                0.00    0.00       4/4           yylp_create_buffer(_IO_FILE *, int) [120]
[118]    0.0    0.00    0.00       4         yy_flex_alloc(unsigned int) [118]
                0.00    0.00       2/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00       2/2           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
[119]    0.0    0.00    0.00       2         initL(void) [119]
                0.00    0.00       2/109         omAllocFromSystem [84]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[120]    0.0    0.00    0.00       2         yylp_create_buffer(_IO_FILE *, int) [120]
                0.00    0.00       4/4           yy_flex_alloc(unsigned int) [118]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [297]
-----------------------------------------------
                0.00    0.00       1/167         global constructors keyed to ff_prime [161]
                0.00    0.00       2/167         initPT(void) [122]
                0.00    0.00      82/167         Array<CanonicalForm>::Array(int) [133]
                0.00    0.00      82/167         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [132]
[121]    0.0    0.00    0.00     167         __builtin_vec_new [121]
                0.00    0.00       1/109         omAllocFromSystem [84]
                0.00    0.00      18/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                                                 <spontaneous>
[122]    0.0    0.00    0.00                 initPT(void) [122]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(int) [133]
                0.00    0.00      82/82          Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [132]
                0.00    0.00       2/167         __builtin_vec_new [121]
                0.00    0.00     156/156         Array<CanonicalForm>::operator[](int) const [4304]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(void) [4307]
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4305]
-----------------------------------------------
                0.00    0.00      69/69          killlocals(int) [124]
[123]    0.0    0.00    0.00      69         killhdl(idrec *, idrec **) [123]
                0.00    0.00      12/965         sleftv::CleanUp(void) [75]
-----------------------------------------------
                0.00    0.00      32/32          iiPStart(idrec *, sleftv *) <cycle 1> [98]
[124]    0.0    0.00    0.00      32         killlocals(int) [124]
                0.00    0.00      69/69          killhdl(idrec *, idrec **) [123]
                0.00    0.00      64/1147        sleftv::Typ(void) [171]
-----------------------------------------------
                0.00    0.00      12/12          iiAssign(sleftv *, sleftv *) [62]
[125]    0.0    0.00    0.00      12         jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      12/965         sleftv::CleanUp(void) [75]
                0.00    0.00      12/340         sleftv::listLength(void) [188]
                0.00    0.00      12/1147        sleftv::Typ(void) [171]
                0.00    0.00      12/32          sleftv::Copy(sleftv *) [227]
                0.00    0.00      12/563         sleftv::Data(void) [175]
                0.00    0.00      12/32          ipMoveId(idrec *) [230]
-----------------------------------------------
                0.00    0.00      11/11          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
[126]    0.0    0.00    0.00      11         iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      11/965         sleftv::CleanUp(void) [75]
                0.00    0.00      22/1147        sleftv::Typ(void) [171]
                0.00    0.00      11/76          iiTestConvert(int, int) [207]
                0.00    0.00      11/72          iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      11/563         sleftv::Data(void) [175]
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [261]
-----------------------------------------------
                0.00    0.00     676/676         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[127]    0.0    0.00    0.00     676         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.00    0.00     792/1258208     nDummy1(snumber **) [42]
                0.00    0.00       9/243268      omAllocBinFromFullPage [20]
                0.00    0.00       3/247738      omFreeToPageFault [30]
                0.00    0.00    1376/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
                0.00    0.00     116/130         npMult(snumber *, snumber *) [198]
                0.00    0.00      58/58          npEqual(snumber *, snumber *) [220]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [64]
[128]    0.0    0.00    0.00       1         yy_create_buffer(_IO_FILE *, int) [128]
                0.00    0.00       2/66          yy_flex_alloc(unsigned int) [88]
                0.00    0.00       1/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [212]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[129]    0.0    0.00    0.00       1         global constructors keyed to fe_promptstr [129]
                0.00    0.00       1/109         omAllocFromSystem [84]
-----------------------------------------------
                                                 <spontaneous>
[130]    0.0    0.00    0.00                 __do_global_ctors_aux [130]
                0.00    0.00       1/1           global constructors keyed to fe_promptstr [129]
                0.00    0.00       1/1           global constructors keyed to feVersionId [4324]
                0.00    0.00       1/1           global constructors keyed to dArith2 [4322]
                0.00    0.00       1/1           global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4341]
                0.00    0.00       1/1           global constructors keyed to sattr::Print(void) [4317]
                0.00    0.00       1/1           global constructors keyed to convSingNClapN(snumber *) [4321]
                0.00    0.00       1/1           global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4337]
                0.00    0.00       1/1           global constructors keyed to omSingOutOfMemoryFunc [4333]
                0.00    0.00       1/1           global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4330]
                0.00    0.00       1/1           global constructors keyed to feOptSpec [4323]
                0.00    0.00       1/1           global constructors keyed to sip_command_bin [4338]
                0.00    0.00       1/1           global constructors keyed to iiCurrArgs [4328]
                0.00    0.00       1/1           global constructors keyed to slists_bin [4339]
                0.00    0.00       1/1           global constructors keyed to rnumber_bin [4334]
                0.00    0.00       1/1           global constructors keyed to ip_smatrix_bin [4329]
                0.00    0.00       1/1           global constructors keyed to MP_INT_bin [4316]
                0.00    0.00       1/1           global constructors keyed to gmp_output_digits [4327]
                0.00    0.00       1/1           global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4340]
                0.00    0.00       1/1           global constructors keyed to fglmUpdatesource(sip_sideal *) [4325]
                0.00    0.00       1/1           global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4320]
                0.00    0.00       1/1           global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4318]
                0.00    0.00       1/1           global constructors keyed to fglmcomb.cc [4326]
                0.00    0.00       1/1           global constructors keyed to s_si_link_extension_bin [4336]
                0.00    0.00       1/1           global constructors keyed to sSubexpr_bin [4335]
                0.00    0.00       1/1           global constructors keyed to mpsr_sleftv_bin [4332]
                0.00    0.00       1/1           global constructors keyed to kBucketCreate(sip_sring *) [4331]
                0.00    0.00       1/1           global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4319]
-----------------------------------------------
                0.00    0.00       3/7047        initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00     192/7047        chainCrit(spolyrec *, int, skStrategy *) [115]
                0.00    0.00     676/7047        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
                0.00    0.00    3066/7047        redEcart(sLObject *, skStrategy *) [11]
                0.00    0.00    3110/7047        redFirst(sLObject *, skStrategy *) [13]
[131]    0.0    0.00    0.00    7047         enterL(sLObject **, int *, int *, sLObject, int) [131]
                0.00    0.00       3/62          omDoRealloc [96]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [122]
[132]    0.0    0.00    0.00      82         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [132]
                0.00    0.00      82/167         __builtin_vec_new [121]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [122]
[133]    0.0    0.00    0.00      82         Array<CanonicalForm>::Array(int) [133]
                0.00    0.00      82/167         __builtin_vec_new [121]
-----------------------------------------------
                0.00    0.00     113/113         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
[134]    0.0    0.00    0.00     113         ksCheckCoeff(snumber **, snumber **) [134]
                0.00    0.00     339/1257408     npIsOne(snumber *) [43]
                0.00    0.00     113/1258208     nDummy1(snumber **) [42]
                0.00    0.00     226/226         ndCopy(snumber *) [191]
                0.00    0.00     113/113         ndGcd(snumber *, snumber *) [202]
-----------------------------------------------
                0.00    0.00       2/13          newFile(char *, _IO_FILE *) [149]
                0.00    0.00       2/13          iiLibCmd(char *, short) [65]
                0.00    0.00       4/13          main [1]
                0.00    0.00       5/13          iiGetLibProcBuffer(procinfo *, int) [139]
[135]    0.0    0.00    0.00      13         feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       5/243268      omAllocBinFromFullPage [20]
                0.00    0.00       5/247738      omFreeToPageFault [30]
                0.00    0.00       8/8           feResource(char, int) [266]
-----------------------------------------------
                0.00    0.00       4/563         updateLHC(skStrategy *) [38]
                0.00    0.00      75/563         chainCrit(spolyrec *, int, skStrategy *) [115]
                0.00    0.00     484/563         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[136]    0.0    0.00    0.00     563         deleteInL(sLObject *, int *, int, skStrategy *) [136]
                0.00    0.00      13/247738      omFreeToPageFault [30]
-----------------------------------------------
                                                 <spontaneous>
[137]    0.0    0.00    0.00                 opendir [137]
                0.00    0.00       3/36          malloc [114]
                0.00    0.00       3/3           calloc [299]
-----------------------------------------------
                0.00    0.00       3/523         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      12/523         updateLHC(skStrategy *) [38]
                0.00    0.00     101/523         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00     407/523         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[138]    0.0    0.00    0.00     523         kBucketCreate(sip_sring *) [138]
                0.00    0.00       5/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00       5/5           iiPStart(idrec *, sleftv *) <cycle 1> [98]
[139]    0.0    0.00    0.00       5         iiGetLibProcBuffer(procinfo *, int) [139]
                0.00    0.00       5/13          feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       2/243268      omAllocBinFromFullPage [20]
                0.00    0.00      10/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [235]
                0.00    0.00       5/5           iiProcArgs(char *, short) [281]
-----------------------------------------------
                0.00    0.00       4/4           newHEdge(spolyrec **, int, skStrategy *) [93]
[140]    0.0    0.00    0.00       4         scComputeHC(sip_sideal *, int, spolyrec *&) [140]
                0.00    0.00       4/4           hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [146]
                0.00    0.00       2/247738      omFreeToPageFault [30]
                0.00    0.00       4/4           hDelete(long **, int) [150]
                0.00    0.00       4/4           hCreate(int) [284]
                0.00    0.00       4/4           hStaircase(long **, int *, int *, int) [289]
                0.00    0.00       4/4           hOrdSupp(long **, int, int *, int) [288]
                0.00    0.00       4/354         hPure(long **, int, int *, int *, int, long *, int *) [184]
                0.00    0.00       4/4           hLexS(long **, int, int *, int) [287]
                0.00    0.00       4/4           hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
                0.00    0.00       4/4           hKill(monrec **, int) [286]
-----------------------------------------------
                                                 <spontaneous>
[141]    0.0    0.00    0.00                 __gmpf_init_set_d [141]
                0.00    0.00       2/36          malloc [114]
-----------------------------------------------
                                                 <spontaneous>
[142]    0.0    0.00    0.00                 __gmpz_init_set_si [142]
                0.00    0.00       2/36          malloc [114]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [41]
[143]    0.0    0.00    0.00       1         iiExport(sleftv *, int) [143]
                0.00    0.00       1/965         sleftv::CleanUp(void) [75]
                0.00    0.00       2/4           idrec::get(char const *, int) [283]
-----------------------------------------------
                0.00    0.00      53/523         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.00    0.00      63/523         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00     407/523         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[144]    0.0    0.00    0.00     523         kBucketDestroy(kBucket **) [144]
                0.00    0.00       6/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00      63/63          enterSMora(sLObject, int, skStrategy *, int) [23]
[145]    0.0    0.00    0.00      63         enterSBba(sLObject, int, skStrategy *, int) [145]
                0.00    0.00       3/4           pEnlargeSet(spolyrec ***, int, int) [148]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[146]    0.0    0.00    0.00       4         hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [146]
                0.00    0.00       2/243268      omAllocBinFromFullPage [20]
                0.00    0.00       4/5           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [280]
-----------------------------------------------
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [256]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [244]
[147]    0.0    0.00    0.00      32         iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
                0.00    0.00      32/32          iiCheckNest(void) [154]
                0.00    0.00      32/33          namerec::push(sip_package *, char *, int, short) [153]
                0.00    0.00      32/32          iiConvName(char *) [229]
                0.00    0.00      32/32          namerec::pop(short) [232]
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [98]
-----------------------------------------------
                0.00    0.00       1/4           idSkipZeroes(sip_sideal *) [156]
                0.00    0.00       3/4           enterSBba(sLObject, int, skStrategy *, int) [145]
[148]    0.0    0.00    0.00       4         pEnlargeSet(spolyrec ***, int, int) [148]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00       1/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00       2/2           main [1]
[149]    0.0    0.00    0.00       2         newFile(char *, _IO_FILE *) [149]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [135]
                0.00    0.00       2/64          __builtin_new [4308]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[150]    0.0    0.00    0.00       4         hDelete(long **, int) [150]
                0.00    0.00       2/247738      omFreeToPageFault [30]
-----------------------------------------------
                0.00    0.00       1/120         yyparse(void) <cycle 1> [41]
                0.00    0.00       1/120         iiLibCmd(char *, short) [65]
                0.00    0.00      41/120         yylplex(char *, char *, lib_style_types *, lp_modes) [67]
                0.00    0.00      77/120         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [109]
[151]    0.0    0.00    0.00     120         enterid(char *, int, int, idrec **, short) [151]
                0.00    0.00     120/120         idrec::set(char *, int, int, short) [152]
-----------------------------------------------
                0.00    0.00     120/120         enterid(char *, int, int, idrec **, short) [151]
[152]    0.0    0.00    0.00     120         idrec::set(char *, int, int, short) [152]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00       2/4           idInit(int, int) [290]
-----------------------------------------------
                0.00    0.00       1/33          main [1]
                0.00    0.00      32/33          iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
[153]    0.0    0.00    0.00      33         namerec::push(sip_package *, char *, int, short) [153]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
[154]    0.0    0.00    0.00      32         iiCheckNest(void) [154]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[155]    0.0    0.00    0.00       1         slStandardInit(void) [155]
                0.00    0.00       1/243268      omAllocBinFromFullPage [20]
                0.00    0.00       1/1           slInitDBMExtension(s_si_link_extension *) [377]
                0.00    0.00       1/1           slInitMPFileExtension(s_si_link_extension *) [378]
                0.00    0.00       1/1           slInitMPTcpExtension(s_si_link_extension *) [379]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[156]    0.0    0.00    0.00       1         idSkipZeroes(sip_sideal *) [156]
                0.00    0.00       1/4           pEnlargeSet(spolyrec ***, int, int) [148]
-----------------------------------------------
                0.00    0.00       3/3           jjJACOB_P(sleftv *, sleftv *) [158]
[157]    0.0    0.00    0.00       3         pDiff(spolyrec *, int) [157]
                0.00    0.00      14/1258208     nDummy1(snumber **) [42]
                0.00    0.00      14/602         npInit(int) [174]
                0.00    0.00      14/130         npMult(snumber *, snumber *) [198]
                0.00    0.00      14/28          npIsZero(snumber *) [234]
                0.00    0.00      14/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[158]    0.0    0.00    0.00       1         jjJACOB_P(sleftv *, sleftv *) [158]
                0.00    0.00       3/3           pDiff(spolyrec *, int) [157]
                0.00    0.00       1/4           idInit(int, int) [290]
                0.00    0.00       1/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[159]    0.0    0.00    0.00       1         idHomIdeal(sip_sideal *, sip_sideal *) [159]
                0.00    0.00       1/1           pIsHomogeneous(spolyrec *) [160]
-----------------------------------------------
                0.00    0.00       1/1           idHomIdeal(sip_sideal *, sip_sideal *) [159]
[160]    0.0    0.00    0.00       1         pIsHomogeneous(spolyrec *) [160]
                0.00    0.00       3/334636      pDeg(spolyrec *, sip_sring *) [44]
-----------------------------------------------
                                                 <spontaneous>
[161]    0.0    0.00    0.00                 global constructors keyed to ff_prime [161]
                0.00    0.00       1/167         __builtin_vec_new [121]
-----------------------------------------------
[162]    0.0    0.00    0.00      12+18      <cycle 2 as a whole> [162]
                0.00    0.00      20             feResource(feResourceConfig_s *, int) <cycle 2> [239]
                0.00    0.00       6             feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
-----------------------------------------------
                0.00    0.00  179398/179398      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[163]    0.0    0.00    0.00  179398         npNeg(snumber *) [163]
-----------------------------------------------
                0.00    0.00      63/6304        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00    6241/6304        message(int, int *, int *, skStrategy *) [165]
[164]    0.0    0.00    0.00    6304         PrintS [164]
-----------------------------------------------
                0.00    0.00    6292/6292        mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[165]    0.0    0.00    0.00    6292         message(int, int *, int *, skStrategy *) [165]
                0.00    0.00    6241/6304        PrintS [164]
                0.00    0.00     118/126         Print [199]
-----------------------------------------------
                0.00    0.00       2/3633        pmInit(char *, short &) [52]
                0.00    0.00       4/3633        newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00       9/3633        pPower(spolyrec *, int) [94]
                0.00    0.00      10/3633        syMake(sleftv *, char *, idrec *) [51]
                0.00    0.00      14/3633        pDiff(spolyrec *, int) [157]
                0.00    0.00     226/3633        ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00     405/3633        hHedge(spolyrec *) [183]
                0.00    0.00    1376/3633        ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
                0.00    0.00    1587/3633        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[166]    0.0    0.00    0.00    3633         p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
-----------------------------------------------
                0.00    0.00    1587/1587        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[167]    0.0    0.00    0.00    1587         pHasNotCF(spolyrec *, spolyrec *) [167]
-----------------------------------------------
                0.00    0.00    1587/1587        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [103]
[168]    0.0    0.00    0.00    1587         pLcm(spolyrec *, spolyrec *, spolyrec *) [168]
-----------------------------------------------
                0.00    0.00    1362/1362        cancelunit(sLObject *) [177]
[169]    0.0    0.00    0.00    1362         rIsPolyVar(int, sip_sring *) [169]
-----------------------------------------------
                0.00    0.00    1297/1297        chainCrit(spolyrec *, int, skStrategy *) [115]
[170]    0.0    0.00    0.00    1297         pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [170]
-----------------------------------------------
                                   4             sleftv::Typ(void) [171]
                0.00    0.00       1/1147        setOption(sleftv *, sleftv *) [376]
                0.00    0.00       1/1147        rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00      12/1147        jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      19/1147        iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      21/1147        iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [113]
                0.00    0.00      22/1147        iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      27/1147        sleftv::Print(sleftv *, int) [112]
                0.00    0.00      32/1147        sleftv::Copy(sleftv *) [227]
                0.00    0.00      43/1147        jjSYSTEM(sleftv *, sleftv *) [236]
                0.00    0.00      44/1147        sleftv::String(void *, short, int) [222]
                0.00    0.00      64/1147        killlocals(int) [124]
                0.00    0.00      67/1147        iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      76/1147        yyparse(void) <cycle 1> [41]
                0.00    0.00     182/1147        jiAssign_1(sleftv *, sleftv *) [63]
                0.00    0.00     206/1147        iiAssign(sleftv *, sleftv *) [62]
                0.00    0.00     330/1147        iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[171]    0.0    0.00    0.00    1147+4       sleftv::Typ(void) [171]
                                   4             sleftv::Typ(void) [171]
-----------------------------------------------
                0.00    0.00     766/766         yylex(MYYSTYPE *) [64]
[172]    0.0    0.00    0.00     766         yy_get_previous_state(void) [172]
-----------------------------------------------
                0.00    0.00       3/697         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      12/697         updateLHC(skStrategy *) [38]
                0.00    0.00     101/697         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [50]
                0.00    0.00     174/697         deleteHC(sLObject *, skStrategy *, short) [24]
                0.00    0.00     407/697         doRed(sLObject *, sTObject *, short, skStrategy *) [12]
[173]    0.0    0.00    0.00     697         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.00       1/602         nInitChar(sip_sring *) [56]
                0.00    0.00      11/602         p_ISet(int, sip_sring *) [259]
                0.00    0.00      14/602         pDiff(spolyrec *, int) [157]
                0.00    0.00     113/602         ndGcd(snumber *, snumber *) [202]
                0.00    0.00     463/602         pNorm(spolyrec *) [15]
[174]    0.0    0.00    0.00     602         npInit(int) [174]
-----------------------------------------------
                                   4             sleftv::Data(void) [175]
                0.00    0.00       1/563         jjSTD(sleftv *, sleftv *) [5]
                0.00    0.00       1/563         jjJACOB_P(sleftv *, sleftv *) [158]
                0.00    0.00       1/563         rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00       2/563         jjTIMES_I(sleftv *, sleftv *, sleftv *) [343]
                0.00    0.00       2/563         jjPLUS_I(sleftv *, sleftv *, sleftv *) [342]
                0.00    0.00       3/563         jjMEMORY(sleftv *, sleftv *) [300]
                0.00    0.00       4/563         jjMakeSub(sleftv *) [292]
                0.00    0.00       7/563         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00       7/563         jjREAD(sleftv *, sleftv *) [270]
                0.00    0.00       8/563         sleftv::CopyD(int) [200]
                0.00    0.00      10/563         jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
                0.00    0.00      11/563         iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      12/563         jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      18/563         jjCOUNT_L(sleftv *, sleftv *) [241]
                0.00    0.00      21/563         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [269]
                0.00    0.00      22/563         jjMINUS_I(sleftv *, sleftv *, sleftv *) [257]
                0.00    0.00      23/563         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [237]
                0.00    0.00      26/563         sleftv::Print(sleftv *, int) [112]
                0.00    0.00      32/563         sleftv::Copy(sleftv *) [227]
                0.00    0.00      36/563         jjGT_I(sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      41/563         jjSYSTEM(sleftv *, sleftv *) [236]
                0.00    0.00      44/563         sleftv::String(void *, short, int) [222]
                0.00    0.00      51/563         yyparse(void) <cycle 1> [41]
                0.00    0.00     180/563         jjPLUS_S(sleftv *, sleftv *, sleftv *) [206]
[175]    0.0    0.00    0.00     563+4       sleftv::Data(void) [175]
                0.00    0.00     431/545         iiCheckRing(int) [176]
                0.00    0.00       2/2           getTimer(void) [314]
                                   4             sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00     114/545         sleftv::CopyD(int) [200]
                0.00    0.00     431/545         sleftv::Data(void) [175]
[176]    0.0    0.00    0.00     545         iiCheckRing(int) [176]
-----------------------------------------------
                0.00    0.00       3/533         initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00      63/533         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00     467/533         updateT(skStrategy *) [29]
[177]    0.0    0.00    0.00     533         cancelunit(sLObject *) [177]
                0.00    0.00    1362/1362        rIsPolyVar(int, sip_sring *) [169]
-----------------------------------------------
                0.00    0.00     502/502         yylex(MYYSTYPE *) [64]
[178]    0.0    0.00    0.00     502         IsCmd(char *, int &) [178]
-----------------------------------------------
                0.00    0.00     467/467         enterT(sLObject, skStrategy *, int) [18]
[179]    0.0    0.00    0.00     467         posInT17(sTObject *, int, sLObject const &) [179]
-----------------------------------------------
                0.00    0.00     425/425         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[180]    0.0    0.00    0.00     425         hStepS(long **, int, int *, int, int *, long *) [180]
-----------------------------------------------
                0.00    0.00     403/403         yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[181]    0.0    0.00    0.00     403         current_pos(int) [181]
-----------------------------------------------
                0.00    0.00     375/375         feReadLine(char *, int) [71]
[182]    0.0    0.00    0.00     375         fePrintEcho(char *, char *) [182]
-----------------------------------------------
                0.00    0.00     354/354         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[183]    0.0    0.00    0.00     354         hHedge(spolyrec *) [183]
                0.00    0.00     405/3633        p_Setm_TotalDegree(spolyrec *, sip_sring *) [166]
-----------------------------------------------
                0.00    0.00       4/354         scComputeHC(sip_sideal *, int, spolyrec *&) [140]
                0.00    0.00     350/354         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[184]    0.0    0.00    0.00     354         hPure(long **, int, int *, int *, int, long *, int *) [184]
-----------------------------------------------
                0.00    0.00     350/350         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[185]    0.0    0.00    0.00     350         hElimS(long **, int *, int, int, int *, int) [185]
-----------------------------------------------
                0.00    0.00     350/350         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[186]    0.0    0.00    0.00     350         hLex2S(long **, int, int, int, int *, int, long **) [186]
-----------------------------------------------
                0.00    0.00     347/347         yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[187]    0.0    0.00    0.00     347         copy_string(lp_modes) [187]
-----------------------------------------------
                0.00    0.00       1/340         rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00      12/340         jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      33/340         jjSTRING_PL(sleftv *, sleftv *) [226]
                0.00    0.00      88/340         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
                0.00    0.00     206/340         iiAssign(sleftv *, sleftv *) [62]
[188]    0.0    0.00    0.00     340         sleftv::listLength(void) [188]
-----------------------------------------------
                0.00    0.00     320/320         syMake(sleftv *, char *, idrec *) [51]
[189]    0.0    0.00    0.00     320         ggetid(char const *, short) [189]
-----------------------------------------------
                0.00    0.00      67/232         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     165/232         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[190]    0.0    0.00    0.00     232         iiTabIndex(sValCmdTab const *, int, int) [190]
-----------------------------------------------
                0.00    0.00     226/226         ksCheckCoeff(snumber **, snumber **) [134]
[191]    0.0    0.00    0.00     226         ndCopy(snumber *) [191]
-----------------------------------------------
                0.00    0.00     197/197         idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [280]
[192]    0.0    0.00    0.00     197         p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [192]
-----------------------------------------------
                0.00    0.00       3/189         omSizeOfAddr [278]
                0.00    0.00     186/189         omSizeOfLargeAddr [194]
[193]    0.0    0.00    0.00     189         malloc_usable_size [193]
-----------------------------------------------
                0.00    0.00       1/186         exitVoice(void) [218]
                0.00    0.00       2/186         yy_flex_free(void *) [295]
                0.00    0.00       3/186         free [224]
                0.00    0.00       5/186         exitBuchMora(skStrategy *) [36]
                0.00    0.00      17/186         omDoRealloc [96]
                0.00    0.00      32/186         yy_flex_free(void *) [213]
                0.00    0.00     126/186         omRealloc0Large [108]
[194]    0.0    0.00    0.00     186         omSizeOfLargeAddr [194]
                0.00    0.00     186/189         malloc_usable_size [193]
-----------------------------------------------
                0.00    0.00       3/177         newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00     174/177         deleteHC(sLObject *, skStrategy *, short) [24]
[195]    0.0    0.00    0.00     177         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [195]
-----------------------------------------------
                0.00    0.00     162/162         omGetUsedBinBytes [302]
[196]    0.0    0.00    0.00     162         omGetUsedBytesOfBin [196]
-----------------------------------------------
                0.00    0.00      61/154         omFreeSizeToSystem [219]
                0.00    0.00      93/154         rEALLOc [90]
[197]    0.0    0.00    0.00     154         fREe [197]
-----------------------------------------------
                0.00    0.00      14/130         pDiff(spolyrec *, int) [157]
                0.00    0.00     116/130         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
[198]    0.0    0.00    0.00     130         npMult(snumber *, snumber *) [198]
-----------------------------------------------
                0.00    0.00       1/126         main [1]
                0.00    0.00       1/126         iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [66]
                0.00    0.00       1/126         messageStat(int, int, int, skStrategy *) [345]
                0.00    0.00       1/126         sleftv::Print(sleftv *, int) [112]
                0.00    0.00       4/126         newHEdge(spolyrec **, int, skStrategy *) [93]
                0.00    0.00     118/126         message(int, int *, int *, skStrategy *) [165]
[199]    0.0    0.00    0.00     126         Print [199]
-----------------------------------------------
                0.00    0.00       1/122         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [339]
                0.00    0.00       1/122         sleftv::Print(sleftv *, int) [112]
                0.00    0.00       2/122         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [107]
                0.00    0.00      10/122         jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
                0.00    0.00      12/122         jjTIMES_P(sleftv *, sleftv *, sleftv *) [87]
                0.00    0.00      12/122         jjPLUS_P(sleftv *, sleftv *, sleftv *) [110]
                0.00    0.00      19/122         iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      65/122         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [76]
[200]    0.0    0.00    0.00     122         sleftv::CopyD(int) [200]
                0.00    0.00     114/545         iiCheckRing(int) [176]
                0.00    0.00       8/563         sleftv::Data(void) [175]
                0.00    0.00       8/8           slInternalCopy(sleftv *, int, void *, _ssubexpr *) [267]
-----------------------------------------------
                0.00    0.00      19/117         syMake(sleftv *, char *, idrec *) [51]
                0.00    0.00      98/117         pmInit(char *, short &) [52]
[201]    0.0    0.00    0.00     117         rIsRingVar(char *) [201]
-----------------------------------------------
                0.00    0.00     113/113         ksCheckCoeff(snumber **, snumber **) [134]
[202]    0.0    0.00    0.00     113         ndGcd(snumber *, snumber *) [202]
                0.00    0.00     113/602         npInit(int) [174]
-----------------------------------------------
                0.00    0.00       1/108         jjPLUS_I(sleftv *, sleftv *, sleftv *) [342]
                0.00    0.00       6/108         jjPLUS_P(sleftv *, sleftv *, sleftv *) [110]
                0.00    0.00      11/108         jjMINUS_I(sleftv *, sleftv *, sleftv *) [257]
                0.00    0.00      90/108         jjPLUS_S(sleftv *, sleftv *, sleftv *) [206]
[203]    0.0    0.00    0.00     108         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [203]
-----------------------------------------------
                0.00    0.00       1/97          yylex(MYYSTYPE *) [64]
                0.00    0.00      32/97          yyrestart(_IO_FILE *) [233]
                0.00    0.00      32/97          myynewbuffer(void) [92]
                0.00    0.00      32/97          myyoldbuffer(void *) [231]
[204]    0.0    0.00    0.00      97         yy_load_buffer_state(void) [204]
-----------------------------------------------
                0.00    0.00       1/91          jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [339]
                0.00    0.00       2/91          jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [107]
                0.00    0.00      23/91          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [237]
                0.00    0.00      65/91          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [76]
[205]    0.0    0.00    0.00      91         sleftv::LData(void) [205]
-----------------------------------------------
                0.00    0.00      90/90          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[206]    0.0    0.00    0.00      90         jjPLUS_S(sleftv *, sleftv *, sleftv *) [206]
                0.00    0.00     180/563         sleftv::Data(void) [175]
                0.00    0.00      90/108         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [203]
-----------------------------------------------
                0.00    0.00      11/76          iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      23/76          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      42/76          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[207]    0.0    0.00    0.00      76         iiTestConvert(int, int) [207]
-----------------------------------------------
                0.00    0.00      75/75          hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[208]    0.0    0.00    0.00      75         hGetmem(int, long **, monrec *) [208]
-----------------------------------------------
                0.00    0.00      75/75          hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
[209]    0.0    0.00    0.00      75         hGetpure(long *) [209]
-----------------------------------------------
                0.00    0.00      11/72          iiWRITE(sleftv *, sleftv *) [126]
                0.00    0.00      23/72          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      38/72          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[210]    0.0    0.00    0.00      72         iiConvert(int, int, int, sleftv *, sleftv *) [210]
                0.00    0.00      19/1147        sleftv::Typ(void) [171]
                0.00    0.00      19/122         sleftv::CopyD(int) [200]
                0.00    0.00      18/18          iiS2Link(void *) [240]
                0.00    0.00       1/1           iiI2P(void *) [330]
-----------------------------------------------
                0.00    0.00      65/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [212]
[211]    0.0    0.00    0.00      65         yy_flush_buffer(yy_buffer_state *) [211]
-----------------------------------------------
                0.00    0.00       1/65          yy_create_buffer(_IO_FILE *, int) [128]
                0.00    0.00      32/65          yyrestart(_IO_FILE *) [233]
                0.00    0.00      32/65          myynewbuffer(void) [92]
[212]    0.0    0.00    0.00      65         yy_init_buffer(yy_buffer_state *, _IO_FILE *) [212]
                0.00    0.00      65/65          yy_flush_buffer(yy_buffer_state *) [211]
-----------------------------------------------
                0.00    0.00      64/64          myyoldbuffer(void *) [231]
[213]    0.0    0.00    0.00      64         yy_flex_free(void *) [213]
                0.00    0.00      32/186         omSizeOfLargeAddr [194]
                0.00    0.00      32/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.00    0.00      63/63          enterSMora(sLObject, int, skStrategy *, int) [23]
[214]    0.0    0.00    0.00      63         HEckeTest(spolyrec *, skStrategy *) [214]
                0.00    0.00      63/63          p_IsPurePower(spolyrec *, sip_sring *) [215]
-----------------------------------------------
                0.00    0.00      63/63          HEckeTest(spolyrec *, skStrategy *) [214]
[215]    0.0    0.00    0.00      63         p_IsPurePower(spolyrec *, sip_sring *) [215]
-----------------------------------------------
                0.00    0.00      63/63          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[216]    0.0    0.00    0.00      63         redtail(spolyrec *, int, skStrategy *) [216]
-----------------------------------------------
                0.00    0.00      62/62          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[217]    0.0    0.00    0.00      62         posInS(spolyrec **, int, spolyrec *) [217]
-----------------------------------------------
                0.00    0.00      16/61          yylex(MYYSTYPE *) [64]
                0.00    0.00      45/61          exitBuffer(feBufferTypes) [228]
[218]    0.0    0.00    0.00      61         exitVoice(void) [218]
                0.00    0.00      61/63          __builtin_delete [4309]
                0.00    0.00      32/32          myyoldbuffer(void *) [231]
                0.00    0.00       1/186         omSizeOfLargeAddr [194]
                0.00    0.00       1/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.00    0.00       1/61          exitVoice(void) [218]
                0.00    0.00       2/61          yy_flex_free(void *) [295]
                0.00    0.00       3/61          free [224]
                0.00    0.00       5/61          exitBuchMora(skStrategy *) [36]
                0.00    0.00      18/61          omFreeBinPagesRegion [245]
                0.00    0.00      32/61          yy_flex_free(void *) [213]
[219]    0.0    0.00    0.00      61         omFreeSizeToSystem [219]
                0.00    0.00      61/154         fREe [197]
-----------------------------------------------
                0.00    0.00      58/58          ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [127]
[220]    0.0    0.00    0.00      58         npEqual(snumber *, snumber *) [220]
-----------------------------------------------
                0.00    0.00      49/49          pmInit(char *, short &) [52]
[221]    0.0    0.00    0.00      49         npRead(char *, snumber **) [221]
-----------------------------------------------
                0.00    0.00      11/44          slWriteAscii(sip_link *, sleftv *) [260]
                0.00    0.00      33/44          jjSTRING_PL(sleftv *, sleftv *) [226]
[222]    0.0    0.00    0.00      44         sleftv::String(void *, short, int) [222]
                0.00    0.00      44/563         sleftv::Data(void) [175]
                0.00    0.00      44/1147        sleftv::Typ(void) [171]
-----------------------------------------------
                0.00    0.00      41/41          yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[223]    0.0    0.00    0.00      41         iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [223]
-----------------------------------------------
                0.00    0.00       6/34          closedir [1013]
                0.00    0.00      28/34          fclose [1248]
[224]    0.0    0.00    0.00      34         free [224]
                0.00    0.00       3/186         omSizeOfLargeAddr [194]
                0.00    0.00       3/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.00    0.00       4/33          feCleanResourceValue(feResourceType, char *) [275]
                0.00    0.00      29/33          feCleanUpPath(char *) [309]
[225]    0.0    0.00    0.00      33         feCleanUpFile(char *) [225]
                0.00    0.00       4/4           mystrcpy(char *, char *) [294]
-----------------------------------------------
                0.00    0.00      33/33          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
[226]    0.0    0.00    0.00      33         jjSTRING_PL(sleftv *, sleftv *) [226]
                0.00    0.00      33/340         sleftv::listLength(void) [188]
                0.00    0.00      33/44          sleftv::String(void *, short, int) [222]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      20/32          yyparse(void) <cycle 1> [41]
[227]    0.0    0.00    0.00      32         sleftv::Copy(sleftv *) [227]
                0.00    0.00      32/1147        sleftv::Typ(void) [171]
                0.00    0.00      32/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00      32/32          yyparse(void) <cycle 1> [41]
[228]    0.0    0.00    0.00      32         exitBuffer(feBufferTypes) [228]
                0.00    0.00      45/61          exitVoice(void) [218]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
[229]    0.0    0.00    0.00      32         iiConvName(char *) [229]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [125]
                0.00    0.00      20/32          iiAssign(sleftv *, sleftv *) [62]
[230]    0.0    0.00    0.00      32         ipMoveId(idrec *) [230]
                0.00    0.00       9/9           lRingDependend(slists *) [263]
-----------------------------------------------
                0.00    0.00      32/32          exitVoice(void) [218]
[231]    0.0    0.00    0.00      32         myyoldbuffer(void *) [231]
                0.00    0.00      64/64          yy_flex_free(void *) [213]
                0.00    0.00      32/97          yy_load_buffer_state(void) [204]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
[232]    0.0    0.00    0.00      32         namerec::pop(short) [232]
-----------------------------------------------
                0.00    0.00      16/32          yylex(MYYSTYPE *) [64]
                0.00    0.00      16/32          yy_get_next_buffer(void) [68]
[233]    0.0    0.00    0.00      32         yyrestart(_IO_FILE *) [233]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [212]
                0.00    0.00      32/97          yy_load_buffer_state(void) [204]
-----------------------------------------------
                0.00    0.00       1/28          _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
                0.00    0.00       2/28          pmInit(char *, short &) [52]
                0.00    0.00      11/28          p_ISet(int, sip_sring *) [259]
                0.00    0.00      14/28          pDiff(spolyrec *, int) [157]
[234]    0.0    0.00    0.00      28         npIsZero(snumber *) [234]
-----------------------------------------------
                0.00    0.00       7/26          slReadAscii2(sip_link *, sleftv *) [271]
                0.00    0.00       9/26          libread(_IO_FILE *, char *, int) [264]
                0.00    0.00      10/26          iiGetLibProcBuffer(procinfo *, int) [139]
[235]    0.0    0.00    0.00      26         myfread(void *, unsigned int, unsigned int, _IO_FILE *) [235]
-----------------------------------------------
                0.00    0.00      25/25          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
[236]    0.0    0.00    0.00      25         jjSYSTEM(sleftv *, sleftv *) [236]
                0.00    0.00      43/1147        sleftv::Typ(void) [171]
                0.00    0.00      41/563         sleftv::Data(void) [175]
                0.00    0.00       2/2           feGetOptIndex(char const *) [310]
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [327]
-----------------------------------------------
                0.00    0.00      23/23          jiAssign_1(sleftv *, sleftv *) [63]
[237]    0.0    0.00    0.00      23         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [237]
                0.00    0.00      23/563         sleftv::Data(void) [175]
                0.00    0.00      23/91          sleftv::LData(void) [205]
-----------------------------------------------
                0.00    0.00       1/20          feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
                0.00    0.00       4/20          feInitResources(char *) [325]
                0.00    0.00       7/20          feSprintf(char *, char const *, int) <cycle 2> [282]
                0.00    0.00       8/20          feResource(char, int) [266]
[238]    0.0    0.00    0.00      20         feGetResourceConfig(char) [238]
-----------------------------------------------
                                   1             feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
                                   7             feSprintf(char *, char const *, int) <cycle 2> [282]
                0.00    0.00       4/12          feInitResources(char *) [325]
                0.00    0.00       8/12          feResource(char, int) [266]
[239]    0.0    0.00    0.00      20         feResource(feResourceConfig_s *, int) <cycle 2> [239]
                                   6             feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
-----------------------------------------------
                0.00    0.00      18/18          iiConvert(int, int, int, sleftv *, sleftv *) [210]
[240]    0.0    0.00    0.00      18         iiS2Link(void *) [240]
                0.00    0.00      18/18          slInit(sip_link *, char *) [250]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[241]    0.0    0.00    0.00      18         jjCOUNT_L(sleftv *, sleftv *) [241]
                0.00    0.00      18/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[242]    0.0    0.00    0.00      18         jjGT_I(sleftv *, sleftv *, sleftv *) [242]
                0.00    0.00      36/563         sleftv::Data(void) [175]
-----------------------------------------------
                                  18             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
[243]    0.0    0.00    0.00      18         jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [243]
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
-----------------------------------------------
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[244]    0.0    0.00    0.00      18         jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [244]
                                  18             iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
-----------------------------------------------
                0.00    0.00      18/18          omFreeBinPages [74]
[245]    0.0    0.00    0.00      18         omFreeBinPagesRegion [245]
                0.00    0.00      18/18          omUnregisterBinPages [246]
                0.00    0.00      18/18          omVfreeToSystem [247]
                0.00    0.00      18/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.00    0.00      18/18          omFreeBinPagesRegion [245]
[246]    0.0    0.00    0.00      18         omUnregisterBinPages [246]
-----------------------------------------------
                0.00    0.00      18/18          omFreeBinPagesRegion [245]
[247]    0.0    0.00    0.00      18         omVfreeToSystem [247]
-----------------------------------------------
                0.00    0.00      18/18          slKill(sip_link *) [251]
[248]    0.0    0.00    0.00      18         slCleanUp(sip_link *) [248]
                0.00    0.00      18/18          slCloseAscii(sip_link *) [249]
-----------------------------------------------
                0.00    0.00      18/18          slCleanUp(sip_link *) [248]
[249]    0.0    0.00    0.00      18         slCloseAscii(sip_link *) [249]
-----------------------------------------------
                0.00    0.00      18/18          iiS2Link(void *) [240]
[250]    0.0    0.00    0.00      18         slInit(sip_link *, char *) [250]
-----------------------------------------------
                0.00    0.00      18/18          sleftv::CleanUp(void) [75]
[251]    0.0    0.00    0.00      18         slKill(sip_link *) [251]
                0.00    0.00      18/18          slCleanUp(sip_link *) [248]
-----------------------------------------------
                0.00    0.00       7/18          slRead(sip_link *, sleftv *) [273]
                0.00    0.00      11/18          slWrite(sip_link *, sleftv *) [261]
[252]    0.0    0.00    0.00      18         slOpenAscii(sip_link *, short) [252]
-----------------------------------------------
                0.00    0.00       1/16          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       3/16          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      12/16          updateLHC(skStrategy *) [38]
[253]    0.0    0.00    0.00      16         sLObject::GetpLength(void) [253]
-----------------------------------------------
                0.00    0.00      16/16          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[254]    0.0    0.00    0.00      16         jjDEFINED(sleftv *, sleftv *) [254]
-----------------------------------------------
                0.00    0.00      14/14          SetProcs(p_Field, p_Length, p_Ord) [322]
[255]    0.0    0.00    0.00      14         FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [255]
                0.00    0.00       2/2           ZeroOrd_2_NonZeroOrd(p_Ord, int) [307]
-----------------------------------------------
                                  14             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[256]    0.0    0.00    0.00      14         jjPROC1(sleftv *, sleftv *) <cycle 1> [256]
                                  14             iiMake_proc(idrec *, sleftv *) <cycle 1> [147]
-----------------------------------------------
                0.00    0.00      11/11          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[257]    0.0    0.00    0.00      11         jjMINUS_I(sleftv *, sleftv *, sleftv *) [257]
                0.00    0.00      22/563         sleftv::Data(void) [175]
                0.00    0.00      11/108         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [203]
-----------------------------------------------
                0.00    0.00      11/11          mALLOc [73]
[258]    0.0    0.00    0.00      11         malloc_extend_top [258]
-----------------------------------------------
                0.00    0.00       1/11          iiI2P(void *) [330]
                0.00    0.00      10/11          syMake(sleftv *, char *, idrec *) [51]
[259]    0.0    0.00    0.00      11         p_ISet(int, sip_sring *) [259]
                0.00    0.00      11/602         npInit(int) [174]
                0.00    0.00      11/28          npIsZero(snumber *) [234]
-----------------------------------------------
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [261]
[260]    0.0    0.00    0.00      11         slWriteAscii(sip_link *, sleftv *) [260]
                0.00    0.00      11/44          sleftv::String(void *, short, int) [222]
-----------------------------------------------
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [126]
[261]    0.0    0.00    0.00      11         slWrite(sip_link *, sleftv *) [261]
                0.00    0.00      11/18          slOpenAscii(sip_link *, short) [252]
                0.00    0.00      11/11          slWriteAscii(sip_link *, sleftv *) [260]
-----------------------------------------------
                0.00    0.00      10/10          jjPOWER_P(sleftv *, sleftv *, sleftv *) [95]
[262]    0.0    0.00    0.00      10         jjOP_REST(sleftv *, sleftv *, sleftv *) [262]
-----------------------------------------------
                0.00    0.00       9/9           ipMoveId(idrec *) [230]
[263]    0.0    0.00    0.00       9         lRingDependend(slists *) [263]
-----------------------------------------------
                0.00    0.00       9/9           yy_get_next_buffer(void) [265]
[264]    0.0    0.00    0.00       9         libread(_IO_FILE *, char *, int) [264]
                0.00    0.00       9/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [235]
-----------------------------------------------
                0.00    0.00       9/9           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[265]    0.0    0.00    0.00       9         yy_get_next_buffer(void) [265]
                0.00    0.00       9/9           libread(_IO_FILE *, char *, int) [264]
                0.00    0.00       2/2           yylprestart(_IO_FILE *) [318]
-----------------------------------------------
                0.00    0.00       8/8           feFopen(char *, char *, char *, int, int) [135]
[266]    0.0    0.00    0.00       8         feResource(char, int) [266]
                0.00    0.00       8/20          feGetResourceConfig(char) [238]
                0.00    0.00       8/12          feResource(feResourceConfig_s *, int) <cycle 2> [239]
-----------------------------------------------
                0.00    0.00       8/8           sleftv::CopyD(int) [200]
[267]    0.0    0.00    0.00       8         slInternalCopy(sleftv *, int, void *, _ssubexpr *) [267]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [273]
[268]    0.0    0.00    0.00       7         sleftv::Eval(void) [268]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [113]
[269]    0.0    0.00    0.00       7         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [269]
                0.00    0.00      21/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[270]    0.0    0.00    0.00       7         jjREAD(sleftv *, sleftv *) [270]
                0.00    0.00       7/563         sleftv::Data(void) [175]
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [273]
-----------------------------------------------
                0.00    0.00       7/7           slReadAscii(sip_link *) [272]
[271]    0.0    0.00    0.00       7         slReadAscii2(sip_link *, sleftv *) [271]
                0.00    0.00       7/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [235]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [273]
[272]    0.0    0.00    0.00       7         slReadAscii(sip_link *) [272]
                0.00    0.00       7/7           slReadAscii2(sip_link *, sleftv *) [271]
-----------------------------------------------
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [270]
[273]    0.0    0.00    0.00       7         slRead(sip_link *, sleftv *) [273]
                0.00    0.00       7/18          slOpenAscii(sip_link *, short) [252]
                0.00    0.00       7/7           slReadAscii(sip_link *) [272]
                0.00    0.00       7/7           sleftv::Eval(void) [268]
-----------------------------------------------
                0.00    0.00       7/7           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[274]    0.0    0.00    0.00       7         yy_get_previous_state(void) [274]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
[275]    0.0    0.00    0.00       6         feCleanResourceValue(feResourceType, char *) [275]
                0.00    0.00       4/33          feCleanUpFile(char *) [225]
                0.00    0.00       2/2           feCleanUpPath(char *) [309]
-----------------------------------------------
                                   6             feResource(feResourceConfig_s *, int) <cycle 2> [239]
[276]    0.0    0.00    0.00       6         feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
                0.00    0.00       6/6           feCleanResourceValue(feResourceType, char *) [275]
                0.00    0.00       6/6           feVerifyResourceValue(feResourceType, char *) [277]
                0.00    0.00       1/1           feGetExpandedExecutable(void) [324]
                0.00    0.00       1/20          feGetResourceConfig(char) [238]
                                   4             feSprintf(char *, char const *, int) <cycle 2> [282]
                                   1             feResource(feResourceConfig_s *, int) <cycle 2> [239]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
[277]    0.0    0.00    0.00       6         feVerifyResourceValue(feResourceType, char *) [277]
-----------------------------------------------
                0.00    0.00       6/6           omDoRealloc [96]
[278]    0.0    0.00    0.00       6         omSizeOfAddr [278]
                0.00    0.00       3/189         malloc_usable_size [193]
-----------------------------------------------
                0.00    0.00       6/6           jjTIMES_P(sleftv *, sleftv *, sleftv *) [87]
[279]    0.0    0.00    0.00       6         p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [279]
-----------------------------------------------
                0.00    0.00       1/5           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       4/5           hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [146]
[280]    0.0    0.00    0.00       5         idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [280]
                0.00    0.00     197/197         p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [192]
-----------------------------------------------
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [139]
[281]    0.0    0.00    0.00       5         iiProcArgs(char *, short) [281]
-----------------------------------------------
                                   4             feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
[282]    0.0    0.00    0.00       4         feSprintf(char *, char const *, int) <cycle 2> [282]
                0.00    0.00       7/20          feGetResourceConfig(char) [238]
                                   7             feResource(feResourceConfig_s *, int) <cycle 2> [239]
-----------------------------------------------
                0.00    0.00       2/4           iiLibCmd(char *, short) [65]
                0.00    0.00       2/4           iiExport(sleftv *, int) [143]
[283]    0.0    0.00    0.00       4         idrec::get(char const *, int) [283]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[284]    0.0    0.00    0.00       4         hCreate(int) [284]
-----------------------------------------------
                                 425             hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[285]    0.0    0.00    0.00       4+425     hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
                0.00    0.00     425/425         hStepS(long **, int, int *, int, int *, long *) [180]
                0.00    0.00     354/354         hHedge(spolyrec *) [183]
                0.00    0.00     350/350         hElimS(long **, int *, int, int, int *, int) [185]
                0.00    0.00     350/354         hPure(long **, int, int *, int *, int, long *, int *) [184]
                0.00    0.00     350/350         hLex2S(long **, int, int, int, int *, int, long **) [186]
                0.00    0.00      75/75          hGetpure(long *) [209]
                0.00    0.00      75/75          hGetmem(int, long **, monrec *) [208]
                                 425             hHedgeStep(long *, long **, int, int *, int, spolyrec *) [285]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[286]    0.0    0.00    0.00       4         hKill(monrec **, int) [286]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[287]    0.0    0.00    0.00       4         hLexS(long **, int, int *, int) [287]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[288]    0.0    0.00    0.00       4         hOrdSupp(long **, int, int *, int) [288]
-----------------------------------------------
                0.00    0.00       4/4           scComputeHC(sip_sideal *, int, spolyrec *&) [140]
[289]    0.0    0.00    0.00       4         hStaircase(long **, int *, int *, int) [289]
-----------------------------------------------
                0.00    0.00       1/4           jjJACOB_P(sleftv *, sleftv *) [158]
                0.00    0.00       1/4           initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00       2/4           idrec::set(char *, int, int, short) [152]
[290]    0.0    0.00    0.00       4         idInit(int, int) [290]
-----------------------------------------------
                0.00    0.00       4/4           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[291]    0.0    0.00    0.00       4         jjINDEX_I(sleftv *, sleftv *, sleftv *) [291]
                0.00    0.00       4/4           jjMakeSub(sleftv *) [292]
-----------------------------------------------
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [291]
[292]    0.0    0.00    0.00       4         jjMakeSub(sleftv *) [292]
                0.00    0.00       4/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00       4/4           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[293]    0.0    0.00    0.00       4         make_version(char *, int) [293]
-----------------------------------------------
                0.00    0.00       4/4           feCleanUpFile(char *) [225]
[294]    0.0    0.00    0.00       4         mystrcpy(char *, char *) [294]
-----------------------------------------------
                0.00    0.00       4/4           reinit_yylp(void) [317]
[295]    0.0    0.00    0.00       4         yy_flex_free(void *) [295]
                0.00    0.00       2/186         omSizeOfLargeAddr [194]
                0.00    0.00       2/61          omFreeSizeToSystem [219]
-----------------------------------------------
                0.00    0.00       4/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [297]
[296]    0.0    0.00    0.00       4         yylp_flush_buffer(yy_buffer_state *) [296]
-----------------------------------------------
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [318]
                0.00    0.00       2/4           yylp_create_buffer(_IO_FILE *, int) [120]
[297]    0.0    0.00    0.00       4         yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [297]
                0.00    0.00       4/4           yylp_flush_buffer(yy_buffer_state *) [296]
-----------------------------------------------
                0.00    0.00       2/4           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [318]
[298]    0.0    0.00    0.00       4         yylp_load_buffer_state(void) [298]
-----------------------------------------------
                0.00    0.00       3/3           opendir [137]
[299]    0.0    0.00    0.00       3         calloc [299]
-----------------------------------------------
                0.00    0.00       3/3           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[300]    0.0    0.00    0.00       3         jjMEMORY(sleftv *, sleftv *) [300]
                0.00    0.00       3/3           omUpdateInfo [303]
                0.00    0.00       3/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00       3/3           omUpdateInfo [303]
[301]    0.0    0.00    0.00       3         malloc_update_mallinfo [301]
-----------------------------------------------
                0.00    0.00       3/3           omUpdateInfo [303]
[302]    0.0    0.00    0.00       3         omGetUsedBinBytes [302]
                0.00    0.00     162/162         omGetUsedBytesOfBin [196]
-----------------------------------------------
                0.00    0.00       3/3           jjMEMORY(sleftv *, sleftv *) [300]
[303]    0.0    0.00    0.00       3         omUpdateInfo [303]
                0.00    0.00       3/3           malloc_update_mallinfo [301]
                0.00    0.00       3/3           omGetUsedBinBytes [302]
-----------------------------------------------
                0.00    0.00       1/3           initSL(sip_sideal *, sip_sideal *, skStrategy *) [61]
                0.00    0.00       2/3           syMake(sleftv *, char *, idrec *) [51]
[304]    0.0    0.00    0.00       3         pIsConstant(spolyrec *) [304]
-----------------------------------------------
                0.00    0.00       3/3           enterT(sLObject, skStrategy *, int) [18]
[305]    0.0    0.00    0.00       3         posInT2(sTObject *, int, sLObject const &) [305]
-----------------------------------------------
                0.00    0.00       1/3           rInit(sleftv *, sleftv *, sleftv *) [53]
                0.00    0.00       2/3           rSetHdl(idrec *, short) [70]
[306]    0.0    0.00    0.00       3         rDBTest(sip_sring *, char *, int) [306]
-----------------------------------------------
                0.00    0.00       2/2           FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [255]
[307]    0.0    0.00    0.00       2         ZeroOrd_2_NonZeroOrd(p_Ord, int) [307]
-----------------------------------------------
                0.00    0.00       2/2           pmInit(char *, short &) [52]
[308]    0.0    0.00    0.00       2         eati(char *, int *) [308]
-----------------------------------------------
                0.00    0.00       2/2           feCleanResourceValue(feResourceType, char *) [275]
[309]    0.0    0.00    0.00       2         feCleanUpPath(char *) [309]
                0.00    0.00      29/33          feCleanUpFile(char *) [225]
-----------------------------------------------
                0.00    0.00       2/2           jjSYSTEM(sleftv *, sleftv *) [236]
[310]    0.0    0.00    0.00       2         feGetOptIndex(char const *) [310]
-----------------------------------------------
                0.00    0.00       2/2           fe_reset_input_mode(void) [312]
[311]    0.0    0.00    0.00       2         fe_reset_fe [311]
-----------------------------------------------
                0.00    0.00       2/2           m2_end [344]
[312]    0.0    0.00    0.00       2         fe_reset_input_mode(void) [312]
                0.00    0.00       2/2           fe_reset_fe [311]
-----------------------------------------------
                0.00    0.00       2/2           __gmpf_clear [4797]
[313]    0.0    0.00    0.00       2         freeSize [313]
-----------------------------------------------
                0.00    0.00       2/2           sleftv::Data(void) [175]
[314]    0.0    0.00    0.00       2         getTimer(void) [314]
-----------------------------------------------
                0.00    0.00       1/2           firstUpdate(skStrategy *) [28]
                0.00    0.00       1/2           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[315]    0.0    0.00    0.00       2         kMoraUseBucket(skStrategy *) [315]
-----------------------------------------------
                0.00    0.00       1/2           global constructors keyed to gmp_output_digits [4327]
                0.00    0.00       1/2           initCanonicalForm(void) [1745]
[316]    0.0    0.00    0.00       2         mmInit(void) [316]
                0.00    0.00       1/1           omInitInfo [352]
-----------------------------------------------
                0.00    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [66]
[317]    0.0    0.00    0.00       2         reinit_yylp(void) [317]
                0.00    0.00       4/4           yy_flex_free(void *) [295]
-----------------------------------------------
                0.00    0.00       2/2           yy_get_next_buffer(void) [265]
[318]    0.0    0.00    0.00       2         yylprestart(_IO_FILE *) [318]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [297]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [298]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [67]
[319]    0.0    0.00    0.00       2         yylpwrap [319]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [53]
[320]    0.0    0.00    0.00       1         IsPrime(int) [320]
-----------------------------------------------
                0.00    0.00       1/1           sleftv::Print(sleftv *, int) [112]
[321]    0.0    0.00    0.00       1         PrintLn [321]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [361]
[322]    0.0    0.00    0.00       1         SetProcs(p_Field, p_Length, p_Ord) [322]
                0.00    0.00      14/14          FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [255]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[323]    0.0    0.00    0.00       1         atGet(sleftv *, char *) [323]
-----------------------------------------------
                0.00    0.00       1/1           feInitResource(feResourceConfig_s *, int) <cycle 2> [276]
[324]    0.0    0.00    0.00       1         feGetExpandedExecutable(void) [324]
                0.00    0.00       1/1           omFindExec [348]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[325]    0.0    0.00    0.00       1         feInitResources(char *) [325]
                0.00    0.00       4/20          feGetResourceConfig(char) [238]
                0.00    0.00       4/12          feResource(feResourceConfig_s *, int) <cycle 2> [239]
-----------------------------------------------
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [327]
[326]    0.0    0.00    0.00       1         feOptAction(feOptIndex) [326]
-----------------------------------------------
                0.00    0.00       1/1           jjSYSTEM(sleftv *, sleftv *) [236]
[327]    0.0    0.00    0.00       1         feSetOptValue(feOptIndex, int) [327]
                0.00    0.00       1/1           feOptAction(feOptIndex) [326]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[328]    0.0    0.00    0.00       1         fe_getopt_long [328]
                0.00    0.00       1/1           _fe_getopt_internal [4342]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [348]
[329]    0.0    0.00    0.00       1         full_readlink [329]
-----------------------------------------------
                0.00    0.00       1/1           iiConvert(int, int, int, sleftv *, sleftv *) [210]
[330]    0.0    0.00    0.00       1         iiI2P(void *) [330]
                0.00    0.00       1/11          p_ISet(int, sip_sring *) [259]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[331]    0.0    0.00    0.00       1         initBuchMoraCrit(skStrategy *) [331]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[332]    0.0    0.00    0.00       1         initBuchMoraPos(skStrategy *) [332]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[333]    0.0    0.00    0.00       1         initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [333]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[334]    0.0    0.00    0.00       1         initMora(sip_sideal *, skStrategy *) [334]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [338]
[335]    0.0    0.00    0.00       1         initRTimer(void) [335]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [338]
[336]    0.0    0.00    0.00       1         initTimer(void) [336]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [338]
[337]    0.0    0.00    0.00       1         init_signals(void) [337]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[338]    0.0    0.00    0.00       1         inits(void) [338]
                0.00    0.00       1/1           init_signals(void) [337]
                0.00    0.00       1/1           initTimer(void) [336]
                0.00    0.00       1/1           initRTimer(void) [335]
                0.00    0.00       1/1           mpsr_Init(void) [346]
-----------------------------------------------
                0.00    0.00       1/1           jiAssign_1(sleftv *, sleftv *) [63]
[339]    0.0    0.00    0.00       1         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [339]
                0.00    0.00       1/122         sleftv::CopyD(int) [200]
                0.00    0.00       1/1           pNormalize(spolyrec *) [356]
                0.00    0.00       1/91          sleftv::LData(void) [205]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[340]    0.0    0.00    0.00       1         jjInitTab1(void) [340]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [104]
[341]    0.0    0.00    0.00       1         jjOPTION_PL(sleftv *, sleftv *) [341]
                0.00    0.00       1/1           setOption(sleftv *, sleftv *) [376]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[342]    0.0    0.00    0.00       1         jjPLUS_I(sleftv *, sleftv *, sleftv *) [342]
                0.00    0.00       2/563         sleftv::Data(void) [175]
                0.00    0.00       1/108         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [203]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[343]    0.0    0.00    0.00       1         jjTIMES_I(sleftv *, sleftv *, sleftv *) [343]
                0.00    0.00       2/563         sleftv::Data(void) [175]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [64]
[344]    0.0    0.00    0.00       1         m2_end [344]
                0.00    0.00       2/2           fe_reset_input_mode(void) [312]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[345]    0.0    0.00    0.00       1         messageStat(int, int, int, skStrategy *) [345]
                0.00    0.00       1/126         Print [199]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [338]
[346]    0.0    0.00    0.00       1         mpsr_Init(void) [346]
-----------------------------------------------
                0.00    0.00       1/1           _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [105]
[347]    0.0    0.00    0.00       1         npAdd(snumber *, snumber *) [347]
-----------------------------------------------
                0.00    0.00       1/1           feGetExpandedExecutable(void) [324]
[348]    0.0    0.00    0.00       1         omFindExec [348]
                0.00    0.00       1/1           omFindExec_link [349]
                0.00    0.00       1/1           full_readlink [329]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [348]
[349]    0.0    0.00    0.00       1         omFindExec_link [349]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [354]
[350]    0.0    0.00    0.00       1         omFreeKeptAddrFromBin [350]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[351]    0.0    0.00    0.00       1         omGetStickyBinOfBin [351]
-----------------------------------------------
                0.00    0.00       1/1           mmInit(void) [316]
[352]    0.0    0.00    0.00       1         omInitInfo [352]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [354]
[353]    0.0    0.00    0.00       1         omIsKnownTopBin [353]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[354]    0.0    0.00    0.00       1         omMergeStickyBinIntoBin [354]
                0.00    0.00       1/1           _omIsOnList [4343]
                0.00    0.00       1/1           omIsKnownTopBin [353]
                0.00    0.00       1/1           omFreeKeptAddrFromBin [350]
                0.00    0.00       1/1           _omRemoveFromList [4344]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[355]    0.0    0.00    0.00       1         omSizeWOfAddr [355]
-----------------------------------------------
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [339]
[356]    0.0    0.00    0.00       1         pNormalize(spolyrec *) [356]
-----------------------------------------------
                0.00    0.00       1/1           rSetHdl(idrec *, short) [70]
[357]    0.0    0.00    0.00       1         pSetGlobals(sip_sring *, short) [357]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [361]
[358]    0.0    0.00    0.00       1         p_FieldIs(sip_sring *) [358]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[359]    0.0    0.00    0.00       1         p_GetSetmProc(sip_sring *) [359]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [361]
[360]    0.0    0.00    0.00       1         p_OrdIs(sip_sring *) [360]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[361]    0.0    0.00    0.00       1         p_SetProcs(sip_sring *, p_Procs_s *) [361]
                0.00    0.00       1/1           p_FieldIs(sip_sring *) [358]
                0.00    0.00       1/1           p_OrdIs(sip_sring *) [360]
                0.00    0.00       1/1           SetProcs(p_Field, p_Length, p_Ord) [322]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
[362]    0.0    0.00    0.00       1         pairs(skStrategy *) [362]
-----------------------------------------------
                0.00    0.00       1/1           nInitChar(sip_sring *) [56]
[363]    0.0    0.00    0.00       1         rFieldType(sip_sring *) [363]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[364]    0.0    0.00    0.00       1         rGetDivMask(int) [364]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[365]    0.0    0.00    0.00       1         rGetExpSize(unsigned long, int &, int) [365]
-----------------------------------------------
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [369]
[366]    0.0    0.00    0.00       1         rOrd_is_Totaldegree_Ordering(sip_sring *) [366]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [41]
[367]    0.0    0.00    0.00       1         rOrderName(char *) [367]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[368]    0.0    0.00    0.00       1         rRightAdjustVarOffset(sip_sring *) [368]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[369]    0.0    0.00    0.00       1         rSetDegStuff(sip_sring *) [369]
                0.00    0.00       1/1           rOrd_is_Totaldegree_Ordering(sip_sring *) [366]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[370]    0.0    0.00    0.00       1         rSetNegWeight(sip_sring *) [370]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [54]
[371]    0.0    0.00    0.00       1         rSetVarL(sip_sring *) [371]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [53]
[372]    0.0    0.00    0.00       1         rSleftvList2StringArray(sleftv *, char **) [372]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [53]
[373]    0.0    0.00    0.00       1         rSleftvOrdering2Ordering(sleftv *, sip_sring *) [373]
-----------------------------------------------
                0.00    0.00       1/1           updateS(short, skStrategy *) [380]
[374]    0.0    0.00    0.00       1         reorderS(int *, skStrategy *) [374]
-----------------------------------------------
                0.00    0.00       1/1           firstUpdate(skStrategy *) [28]
[375]    0.0    0.00    0.00       1         reorderT(skStrategy *) [375]
-----------------------------------------------
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [341]
[376]    0.0    0.00    0.00       1         setOption(sleftv *, sleftv *) [376]
                0.00    0.00       1/1147        sleftv::Typ(void) [171]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [155]
[377]    0.0    0.00    0.00       1         slInitDBMExtension(s_si_link_extension *) [377]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [155]
[378]    0.0    0.00    0.00       1         slInitMPFileExtension(s_si_link_extension *) [378]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [155]
[379]    0.0    0.00    0.00       1         slInitMPTcpExtension(s_si_link_extension *) [379]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [60]
[380]    0.0    0.00    0.00       1         updateS(short, skStrategy *) [380]
                0.00    0.00       1/1           reorderS(int *, skStrategy *) [374]
-----------------------------------------------
                0.00    0.00     156/156         initPT(void) [122]
[4304]   0.0    0.00    0.00     156         Array<CanonicalForm>::operator[](int) const [4304]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [122]
[4305]   0.0    0.00    0.00      82         Array<CanonicalForm>::~Array(void) [4305]
                0.00    0.00      82/82          __builtin_vec_delete [4306]
-----------------------------------------------
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4305]
[4306]   0.0    0.00    0.00      82         __builtin_vec_delete [4306]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [122]
[4307]   0.0    0.00    0.00      82         Array<CanonicalForm>::Array(void) [4307]
-----------------------------------------------
                0.00    0.00       1/64          yyparse(void) <cycle 1> [41]
                0.00    0.00       1/64          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       2/64          newFile(char *, _IO_FILE *) [149]
                0.00    0.00      60/64          newBuffer(char *, feBufferTypes, procinfo *, int) [91]
[4308]   0.0    0.00    0.00      64         __builtin_new [4308]
-----------------------------------------------
                0.00    0.00       1/63          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/63          sleftv::CleanUp(void) [75]
                0.00    0.00      61/63          exitVoice(void) [218]
[4309]   0.0    0.00    0.00      63         __builtin_delete [4309]
-----------------------------------------------
                0.00    0.00      57/57          omAllocNewBinPagesRegion [85]
[4310]   0.0    0.00    0.00      57         _omVallocFromSystem [4310]
-----------------------------------------------
                0.00    0.00       1/42          global constructors keyed to sattr::Print(void) [4317]
                0.00    0.00       1/42          global constructors keyed to slists_bin [4339]
                0.00    0.00       1/42          global constructors keyed to rnumber_bin [4334]
                0.00    0.00       1/42          global constructors keyed to ip_smatrix_bin [4329]
                0.00    0.00       1/42          global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4340]
                0.00    0.00       1/42          rComplete(sip_sring *, int) [54]
                0.00    0.00       1/42          global constructors keyed to mpsr_sleftv_bin [4332]
                0.00    0.00       1/42          global constructors keyed to kBucketCreate(sip_sring *) [4331]
                0.00    0.00       1/42          global constructors keyed to InternalInteger::InternalInteger_bin [4444]
                0.00    0.00       2/42          global constructors keyed to term::term_bin [4446]
                0.00    0.00       3/42          global constructors keyed to s_si_link_extension_bin [4336]
                0.00    0.00       5/42          global constructors keyed to sSubexpr_bin [4335]
                0.00    0.00       6/42          global constructors keyed to sip_command_bin [4338]
                0.00    0.00      17/42          global constructors keyed to MP_INT_bin [4316]
[4311]   0.0    0.00    0.00      42         _omGetSpecBin [4311]
                0.00    0.00      14/14          _omFindInSortedList [4312]
                0.00    0.00       6/6           _omInsertInSortedList [4313]
-----------------------------------------------
                0.00    0.00      14/14          _omGetSpecBin [4311]
[4312]   0.0    0.00    0.00      14         _omFindInSortedList [4312]
-----------------------------------------------
                0.00    0.00       6/6           _omGetSpecBin [4311]
[4313]   0.0    0.00    0.00       6         _omInsertInSortedList [4313]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4772]
[4314]   0.0    0.00    0.00       1         global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4314]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4772]
[4315]   0.0    0.00    0.00       1         global destructors keyed to gmp_output_digits [4315]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4316]   0.0    0.00    0.00       1         global constructors keyed to MP_INT_bin [4316]
                0.00    0.00      17/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4317]   0.0    0.00    0.00       1         global constructors keyed to sattr::Print(void) [4317]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4318]   0.0    0.00    0.00       1         global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4318]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4319]   0.0    0.00    0.00       1         global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4319]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4320]   0.0    0.00    0.00       1         global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4320]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4321]   0.0    0.00    0.00       1         global constructors keyed to convSingNClapN(snumber *) [4321]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4322]   0.0    0.00    0.00       1         global constructors keyed to dArith2 [4322]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4323]   0.0    0.00    0.00       1         global constructors keyed to feOptSpec [4323]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4324]   0.0    0.00    0.00       1         global constructors keyed to feVersionId [4324]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4325]   0.0    0.00    0.00       1         global constructors keyed to fglmUpdatesource(sip_sideal *) [4325]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4326]   0.0    0.00    0.00       1         global constructors keyed to fglmcomb.cc [4326]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4327]   0.0    0.00    0.00       1         global constructors keyed to gmp_output_digits [4327]
                0.00    0.00       1/2           mmInit(void) [316]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4328]   0.0    0.00    0.00       1         global constructors keyed to iiCurrArgs [4328]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4329]   0.0    0.00    0.00       1         global constructors keyed to ip_smatrix_bin [4329]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4330]   0.0    0.00    0.00       1         global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4330]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4331]   0.0    0.00    0.00       1         global constructors keyed to kBucketCreate(sip_sring *) [4331]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4332]   0.0    0.00    0.00       1         global constructors keyed to mpsr_sleftv_bin [4332]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4333]   0.0    0.00    0.00       1         global constructors keyed to omSingOutOfMemoryFunc [4333]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4334]   0.0    0.00    0.00       1         global constructors keyed to rnumber_bin [4334]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4335]   0.0    0.00    0.00       1         global constructors keyed to sSubexpr_bin [4335]
                0.00    0.00       5/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4336]   0.0    0.00    0.00       1         global constructors keyed to s_si_link_extension_bin [4336]
                0.00    0.00       3/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4337]   0.0    0.00    0.00       1         global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4337]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4338]   0.0    0.00    0.00       1         global constructors keyed to sip_command_bin [4338]
                0.00    0.00       6/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4339]   0.0    0.00    0.00       1         global constructors keyed to slists_bin [4339]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4340]   0.0    0.00    0.00       1         global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4340]
                0.00    0.00       1/42          _omGetSpecBin [4311]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [130]
[4341]   0.0    0.00    0.00       1         global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4341]
-----------------------------------------------
                0.00    0.00       1/1           fe_getopt_long [328]
[4342]   0.0    0.00    0.00       1         _fe_getopt_internal [4342]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [354]
[4343]   0.0    0.00    0.00       1         _omIsOnList [4343]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [354]
[4344]   0.0    0.00    0.00       1         _omRemoveFromList [4344]
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

  [75] sleftv::CleanUp(void) [280] idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [348] omFindExec
 [200] sleftv::CopyD(int)    [156] idSkipZeroes(sip_sideal *) [349] omFindExec_link (omFindExec.c)
 [227] sleftv::Copy(sleftv *) [106] id_Delete(sip_sideal **, sip_sring *) [74] omFreeBinPages
 [175] sleftv::Data(void)     [62] iiAssign(sleftv *, sleftv *) [245] omFreeBinPagesRegion (omBinPage.c)
 [268] sleftv::Eval(void)    [154] iiCheckNest(void)     [350] omFreeKeptAddrFromBin
 [255] FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [176] iiCheckRing(int) [219] omFreeSizeToSystem
 [253] sLObject::GetpLength(void) [229] iiConvName(char *) [30] omFreeToPageFault
 [214] HEckeTest(spolyrec *, skStrategy *) [210] iiConvert(int, int, int, sleftv *, sleftv *) [351] omGetStickyBinOfBin
 [178] IsCmd(char *, int &)  [109] iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [302] omGetUsedBinBytes
 [320] IsPrime(int)          [143] iiExport(sleftv *, int) [196] omGetUsedBytesOfBin (omBin.c)
 [205] sleftv::LData(void)     [4] iiExprArith1(sleftv *, sleftv *, int) [352] omInitInfo
 [199] Print                  [83] iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) [353] omIsKnownTopBin
 [321] PrintLn               [113] iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [354] omMergeStickyBinIntoBin
 [164] PrintS                [104] iiExprArithM(sleftv *, sleftv *, int) [108] omRealloc0Large
 [112] sleftv::Print(sleftv *, int) [139] iiGetLibProcBuffer(procinfo *, int) [89] omReallocSizeFromSystem
 [322] SetProcs(p_Field, p_Length, p_Ord) [330] iiI2P(void *) [102] omRegisterBinPages (omBinPage.c)
 [222] sleftv::String(void *, short, int) [223] iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [278] omSizeOfAddr
 [171] sleftv::Typ(void)      [65] iiLibCmd(char *, short) [194] omSizeOfLargeAddr
 [307] ZeroOrd_2_NonZeroOrd(p_Ord, int) [66] iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [355] omSizeWOfAddr
 [4305] Array<CanonicalForm>::~Array(void) [147] iiMake_proc(idrec *, sleftv *) [246] omUnregisterBinPages (omBinPage.c)
 [4314] global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [98] iiPStart(idrec *, sleftv *) [303] omUpdateInfo
 [4315] global destructors keyed to gmp_output_digits [86] iiParameter(sleftv *) [247] omVfreeToSystem
 [4316] global constructors keyed to MP_INT_bin [281] iiProcArgs(char *, short) [170] pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
 [4317] global constructors keyed to sattr::Print(void) [240] iiS2Link(void *) [44] pDeg(spolyrec *, sip_sring *)
 [4318] global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [190] iiTabIndex(sValCmdTab const *, int, int) [157] pDiff(spolyrec *, int)
 [4319] global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [207] iiTestConvert(int, int) [148] pEnlargeSet(spolyrec ***, int, int)
 [4320] global constructors keyed to idealFunctionals::idealFunctionals(int, int) [126] iiWRITE(sleftv *, sleftv *) [167] pHasNotCF(spolyrec *, spolyrec *)
 [4321] global constructors keyed to convSingNClapN(snumber *) [331] initBuchMoraCrit(skStrategy *) [304] pIsConstant(spolyrec *)
 [4322] global constructors keyed to dArith2 [332] initBuchMoraPos(skStrategy *) [160] pIsHomogeneous(spolyrec *)
 [4323] global constructors keyed to feOptSpec [60] initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [45] pLDeg0c(spolyrec *, int *, sip_sring *)
 [4324] global constructors keyed to feVersionId [99] initEcartNormal(sLObject *) [168] pLcm(spolyrec *, spolyrec *, spolyrec *)
 [129] global constructors keyed to fe_promptstr [111] initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [32] pLength(spolyrec *)
 [4325] global constructors keyed to fglmUpdatesource(sip_sideal *) [333] initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [15] pNorm(spolyrec *)
 [4326] global constructors keyed to fglmcomb.cc [119] initL(void) [356] pNormalize(spolyrec *)
 [4327] global constructors keyed to gmp_output_digits [334] initMora(sip_sideal *, skStrategy *) [94] pPower(spolyrec *, int)
 [4328] global constructors keyed to iiCurrArgs [335] initRTimer(void) [357] pSetGlobals(sip_sring *, short)
 [4329] global constructors keyed to ip_smatrix_bin [61] initSL(sip_sideal *, sip_sideal *, skStrategy *) [14] p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
 [4330] global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [336] initTimer(void) [21] p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
 [4331] global constructors keyed to kBucketCreate(sip_sring *) [337] init_signals(void) [33] p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
 [4332] global constructors keyed to mpsr_sleftv_bin [100] initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [358] p_FieldIs(sip_sring *)
 [4333] global constructors keyed to omSingOutOfMemoryFunc [338] inits(void) [359] p_GetSetmProc(sip_sring *)
 [4334] global constructors keyed to rnumber_bin [230] ipMoveId(idrec *) [34] p_GetShortExpVector(spolyrec *, sip_sring *)
 [4335] global constructors keyed to sSubexpr_bin [107] jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [259] p_ISet(int, sip_sring *)
 [4336] global constructors keyed to s_si_link_extension_bin [237] jiA_INT(sleftv *, sleftv *, _ssubexpr *) [215] p_IsPurePower(spolyrec *, sip_sring *)
 [4337] global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [339] jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [195] p_LmCmp(spolyrec *, spolyrec *, sip_sring *)
 [4338] global constructors keyed to sip_command_bin [76] jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [192] p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
 [4339] global constructors keyed to slists_bin [63] jiAssign_1(sleftv *, sleftv *) [10] p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
 [4340] global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [125] jjA_L_LIST(sleftv *, sleftv *) [279] p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
 [4341] global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [269] jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [360] p_OrdIs(sip_sring *)
  [82] _IO_do_write          [241] jjCOUNT_L(sleftv *, sleftv *) [361] p_SetProcs(sip_sring *, p_Procs_s *)
  [49] _IO_file_write        [254] jjDEFINED(sleftv *, sleftv *) [166] p_Setm_TotalDegree(spolyrec *, sip_sring *)
  [59] _IO_file_xsputn       [242] jjGT_I(sleftv *, sleftv *, sleftv *) [19] p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
 [132] Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [291] jjINDEX_I(sleftv *, sleftv *, sleftv *) [26] p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket
 [4309] __builtin_delete     [340] jjInitTab1(void)      [362] pairs(skStrategy *)
 [4308] __builtin_new        [158] jjJACOB_P(sleftv *, sleftv *) [52] pmInit(char *, short &)
 [4306] __builtin_vec_delete [243] jjKLAMMER_PL(sleftv *, sleftv *) [232] namerec::pop(short)
 [121] __builtin_vec_new     [300] jjMEMORY(sleftv *, sleftv *) [35] posInL17(sLObject *, int, sLObject *, skStrategy *)
 (4918) __mcount_internal    [257] jjMINUS_I(sleftv *, sleftv *, sleftv *) [217] posInS(spolyrec **, int, spolyrec *)
 [4307] Array<CanonicalForm>::Array(void) [292] jjMakeSub(sleftv *) [179] posInT17(sTObject *, int, sLObject const &)
 [133] Array<CanonicalForm>::Array(int) [341] jjOPTION_PL(sleftv *, sleftv *) [305] posInT2(sTObject *, int, sLObject const &)
 [4304] Array<CanonicalForm>::operator[](int) const [262] jjOP_REST(sleftv *, sleftv *, sleftv *) [16] pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec
 [4342] _fe_getopt_internal  [203] jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [17] pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
 [4312] _omFindInSortedList  [342] jjPLUS_I(sleftv *, sleftv *, sleftv *) [153] namerec::push(sip_package *, char *, int, short)
 [4311] _omGetSpecBin        [110] jjPLUS_P(sleftv *, sleftv *, sleftv *) [54] rComplete(sip_sring *, int)
 [4313] _omInsertInSortedList [206] jjPLUS_S(sleftv *, sleftv *, sleftv *) [306] rDBTest(sip_sring *, char *, int)
 [4343] _omIsOnList           [95] jjPOWER_P(sleftv *, sleftv *, sleftv *) [90] rEALLOc
 [4344] _omRemoveFromList    [256] jjPROC1(sleftv *, sleftv *) [363] rFieldType(sip_sring *)
 [4310] _omVallocFromSystem  [244] jjPROC(sleftv *, sleftv *, sleftv *) [364] rGetDivMask(int)
 [105] _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [270] jjREAD(sleftv *, sleftv *) [365] rGetExpSize(unsigned long, int &, int)
 [323] atGet(sleftv *, char *) [5] jjSTD(sleftv *, sleftv *) [53] rInit(sleftv *, sleftv *, sleftv *)
 [299] calloc                [226] jjSTRING_PL(sleftv *, sleftv *) [169] rIsPolyVar(int, sip_sring *)
 [177] cancelunit(sLObject *) [236] jjSYSTEM(sleftv *, sleftv *) [201] rIsRingVar(char *)
 [115] chainCrit(spolyrec *, int, skStrategy *) [343] jjTIMES_I(sleftv *, sleftv *, sleftv *) [366] rOrd_is_Totaldegree_Ordering(sip_sring *)
  [37] cleanT(skStrategy *)   [87] jjTIMES_P(sleftv *, sleftv *, sleftv *) [367] rOrderName(char *)
 [187] copy_string(lp_modes)  [22] kBucketCanonicalize(kBucket *) [368] rRightAdjustVarOffset(sip_sring *)
 [181] current_pos(int)       [31] kBucketClear(kBucket *, spolyrec **, int *) [369] rSetDegStuff(sip_sring *)
  [24] deleteHC(sLObject *, skStrategy *, short) [138] kBucketCreate(sip_sring *) [70] rSetHdl(idrec *, short)
 [136] deleteInL(sLObject *, int *, int, skStrategy *) [144] kBucketDestroy(kBucket **) [370] rSetNegWeight(sip_sring *)
  [12] doRed(sLObject *, sTObject *, short, skStrategy *) [173] kBucketInit(kBucket *, spolyrec *, int) [371] rSetVarL(sip_sring *)
 [308] eati(char *, int *)     [9] kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [372] rSleftvList2StringArray(sleftv *, char **)
 [131] enterL(sLObject **, int *, int *, sLObject, int) [48] kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [373] rSleftvOrdering2Ordering(sleftv *, sip_sring *)
 [103] enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [25] kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [11] redEcart(sLObject *, skStrategy *)
 [145] enterSBba(sLObject, int, skStrategy *, int) [315] kMoraUseBucket(skStrategy *) [13] redFirst(sLObject *, skStrategy *)
  [23] enterSMora(sLObject, int, skStrategy *, int) [6] kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [216] redtail(spolyrec *, int, skStrategy *)
  [18] enterT(sLObject, skStrategy *, int) [123] killhdl(idrec *, idrec **) [317] reinit_yylp(void)
 [151] enterid(char *, int, int, idrec **, short) [124] killlocals(int) [116] reorderL(skStrategy *)
  [97] enterpairs(spolyrec *, int, int, int, skStrategy *, int) [134] ksCheckCoeff(snumber **, snumber **) [374] reorderS(int *, skStrategy *)
  [36] exitBuchMora(skStrategy *) [127] ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [375] reorderT(skStrategy *)
 [228] exitBuffer(feBufferTypes) [50] ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [140] scComputeHC(sip_sideal *, int, spolyrec *&)
 [218] exitVoice(void)         [8] ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [376] setOption(sleftv *, sleftv *)
 [197] fREe                  [263] lRingDependend(slists *) [152] idrec::set(char *, int, int, short)
 [275] feCleanResourceValue(feResourceType, char *) [264] libread(_IO_FILE *, char *, int) [248] slCleanUp(sip_link *)
 [225] feCleanUpFile(char *) [188] sleftv::listLength(void) [249] slCloseAscii(sip_link *)
 [309] feCleanUpPath(char *) [344] m2_end                [377] slInitDBMExtension(s_si_link_extension *)
 [135] feFopen(char *, char *, char *, int, int) [73] mALLOc [378] slInitMPFileExtension(s_si_link_extension *)
 [324] feGetExpandedExecutable(void) [1] main            [379] slInitMPTcpExtension(s_si_link_extension *)
 [310] feGetOptIndex(char const *) [293] make_version(char *, int) [250] slInit(sip_link *, char *)
 [238] feGetResourceConfig(char) [114] malloc            [267] slInternalCopy(sleftv *, int, void *, _ssubexpr *)
 [276] feInitResource(feResourceConfig_s *, int) [258] malloc_extend_top (omMalloc.c) [251] slKill(sip_link *)
 [325] feInitResources(char *) [301] malloc_update_mallinfo [252] slOpenAscii(sip_link *, short)
 [326] feOptAction(feOptIndex) [193] malloc_usable_size  [271] slReadAscii2(sip_link *, sleftv *)
 [182] fePrintEcho(char *, char *) (2381) mcount         [272] slReadAscii(sip_link *)
  [71] feReadLine(char *, int) [58] memmove              [273] slRead(sip_link *, sleftv *)
 [239] feResource(feResourceConfig_s *, int) [79] memset [155] slStandardInit(void)
 [266] feResource(char, int) [345] messageStat(int, int, int, skStrategy *) [260] slWriteAscii(sip_link *, sleftv *)
 [327] feSetOptValue(feOptIndex, int) [165] message(int, int *, int *, skStrategy *) [261] slWrite(sip_link *, sleftv *)
 [282] feSprintf(char *, char const *, int) [316] mmInit(void) [80] strcat
 [277] feVerifyResourceValue(feResourceType, char *) [7] mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [51] syMake(sleftv *, char *, idrec *)
 [328] fe_getopt_long        [346] mpsr_Init(void)        [81] system
 [311] fe_reset_fe           [235] myfread(void *, unsigned int, unsigned int, _IO_FILE *) [38] updateLHC(skStrategy *)
 [312] fe_reset_input_mode(void) [294] mystrcpy(char *, char *) [380] updateS(short, skStrategy *)
  [46] fflush                 [92] myynewbuffer(void)     [29] updateT(skStrategy *)
  [28] firstUpdate(skStrategy *) [231] myyoldbuffer(void *) [39] write
  [77] flockfile              [42] nDummy1(snumber **)   [128] yy_create_buffer(_IO_FILE *, int)
  [47] fork                   [57] nDummy2(snumber *&)    [88] yy_flex_alloc(unsigned int)
 [224] free                   [56] nInitChar(sip_sring *) [118] yy_flex_alloc(unsigned int)
 [313] freeSize               [69] nSetChar(sip_sring *, short) [213] yy_flex_free(void *)
 [329] full_readlink (omFindExec.c) [191] ndCopy(snumber *) [295] yy_flex_free(void *)
  [78] fwrite                [202] ndGcd(snumber *, snumber *) [211] yy_flush_buffer(yy_buffer_state *)
 [314] getTimer(void)         [91] newBuffer(char *, feBufferTypes, procinfo *, int) [68] yy_get_next_buffer(void)
 [283] idrec::get(char const *, int) [149] newFile(char *, _IO_FILE *) [265] yy_get_next_buffer(void)
 [189] ggetid(char const *, short) [93] newHEdge(spolyrec **, int, skStrategy *) [172] yy_get_previous_state(void)
 [284] hCreate(int)          [347] npAdd(snumber *, snumber *) [274] yy_get_previous_state(void)
 [150] hDelete(long **, int)  [40] npDiv(snumber *, snumber *) [212] yy_init_buffer(yy_buffer_state *, _IO_FILE *)
 [185] hElimS(long **, int *, int, int, int *, int) [220] npEqual(snumber *, snumber *) [204] yy_load_buffer_state(void)
 [208] hGetmem(int, long **, monrec *) [55] npInitChar(int, sip_sring *) [64] yylex(MYYSTYPE *)
 [209] hGetpure(long *)      [174] npInit(int)           [120] yylp_create_buffer(_IO_FILE *, int)
 [285] hHedgeStep(long *, long **, int, int *, int, spolyrec *) [43] npIsOne(snumber *) [296] yylp_flush_buffer(yy_buffer_state *)
 [183] hHedge(spolyrec *)    [234] npIsZero(snumber *)   [297] yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
 [146] hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [198] npMult(snumber *, snumber *) [298] yylp_load_buffer_state(void)
 [286] hKill(monrec **, int) [163] npNeg(snumber *)       [67] yylplex(char *, char *, lib_style_types *, lp_modes)
 [186] hLex2S(long **, int, int, int, int *, int, long **) [221] npRead(char *, snumber **) [318] yylprestart(_IO_FILE *)
 [287] hLexS(long **, int, int *, int) [72] npSetChar(int, sip_sring *) [319] yylpwrap
 [288] hOrdSupp(long **, int, int *, int) [20] omAllocBinFromFullPage [41] yyparse(void)
 [184] hPure(long **, int, int *, int *, int, long *, int *) [27] omAllocBinPage [233] yyrestart(_IO_FILE *)
 [289] hStaircase(long **, int *, int *, int) [84] omAllocFromSystem [3] <cycle 1>
 [180] hStepS(long **, int, int *, int, int *, long *) [85] omAllocNewBinPagesRegion (omBinPage.c) [162] <cycle 2>
 [159] idHomIdeal(sip_sideal *, sip_sideal *) [101] omBinPageIndexFault (omBinPage.c)
 [290] idInit(int, int)       [96] omDoRealloc
