Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 58.13     18.49    18.49   171074     0.11     0.12  p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
 16.13     23.62     5.13    39961     0.13     0.14  p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
  4.15     24.94     1.32   130473     0.01     0.01  pLength(spolyrec *)
  3.55     26.07     1.13   215976     0.01     0.01  pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec
  2.55     26.88     0.81                             __mcount_internal
  1.63     27.40     0.52   250205     0.00     0.00  kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int)
  1.35     27.83     0.43   236720     0.00     0.00  omFreeToPageFault
  1.13     28.19     0.36   250203     0.00     0.00  p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket
  1.07     28.53     0.34    10212     0.03     2.48  redFirst(sLObject *, skStrategy *)
  1.04     28.86     0.33                             mcount
  1.04     29.19     0.33                             write
  0.79     29.44     0.25   260805     0.00     0.00  p_GetShortExpVector(spolyrec *, sip_sring *)
  0.66     29.65     0.21      161     1.30     1.83  pNorm(spolyrec *)
  0.63     29.85     0.20   237049     0.00     0.00  omAllocBinFromFullPage
  0.57     30.03     0.18      352     0.51     1.51  deleteHC(sLObject *, skStrategy *, short)
  0.50     30.19     0.16   250203     0.00     0.11  kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *)
  0.50     30.35     0.16     3391     0.05     0.05  omAllocBinPage
  0.44     30.49     0.14    23744     0.01     0.01  pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
  0.41     30.62     0.13   250155     0.00     0.11  ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *)
  0.38     30.74     0.12      259     0.46     0.49  p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
  0.38     30.86     0.12      198     0.61     0.61  pLDeg0c(spolyrec *, int *, sip_sring *)
  0.38     30.98     0.12      164     0.73     0.80  p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
  0.31     31.08     0.10    44860     0.00     0.00  posInL17(sLObject *, int, sLObject *, skStrategy *)
  0.31     31.18     0.10      432     0.23     9.14  redEcart(sLObject *, skStrategy *)
  0.28     31.27     0.09   251095     0.00     0.00  pDeg(spolyrec *, sip_sring *)
  0.16     31.32     0.05   440834     0.00     0.00  npIsOne(snumber *)
  0.16     31.37     0.05      240     0.21     0.22  p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
  0.13     31.41     0.04        1    40.00 30103.99  mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *)
  0.13     31.45     0.04                             _IO_file_xsputn
  0.09     31.48     0.03   441132     0.00     0.00  nDummy1(snumber **)
  0.09     31.51     0.03   190279     0.00     0.00  npDiv(snumber *, snumber *)
  0.09     31.54     0.03   158314     0.00     0.00  npNeg(snumber *)
  0.06     31.56     0.02   190418     0.00     0.00  nDummy2(snumber *&)
  0.06     31.58     0.02    28191     0.00     0.13  doRed(sLObject *, sTObject *, short, skStrategy *)
  0.06     31.60     0.02                             fork
  0.06     31.62     0.02                             fwrite
  0.06     31.64     0.02                             memmove
  0.03     31.65     0.01    10932     0.00     0.00  enterL(sLObject **, int *, int *, sLObject, int)
  0.03     31.66     0.01    10655     0.00     0.00  PrintS
  0.03     31.67     0.01    10644     0.00     0.00  message(int, int *, int *, skStrategy *)
  0.03     31.68     0.01    10556     0.00     0.00  kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *)
  0.03     31.69     0.01      329     0.03     0.03  ggetid(char const *, short)
  0.03     31.70     0.01      162     0.06     0.06  omGetUsedBytesOfBin
  0.03     31.71     0.01      118     0.08     0.08  Print
  0.03     31.72     0.01       76     0.13     0.13  omAllocFromSystem
  0.03     31.73     0.01       33     0.30     2.18  yyparse(void)
  0.03     31.74     0.01        3     3.33     3.33  rDBTest(sip_sring *, char *, int)
  0.03     31.75     0.01        2     5.00     5.13  yylplex(char *, char *, lib_style_types *, lp_modes)
  0.03     31.76     0.01        1    10.00    10.26  npInitChar(int, sip_sring *)
  0.03     31.77     0.01        1    10.00    10.26  npSetChar(int, sip_sring *)
  0.03     31.78     0.01                             _IO_do_write
  0.03     31.79     0.01                             _IO_file_sync
  0.03     31.80     0.01                             _IO_file_write
  0.03     31.81     0.01                             _IO_link_in
  0.00     31.81     0.00     1820     0.00     0.00  yylex(MYYSTYPE *)
  0.00     31.81     0.00     1477     0.00     0.00  p_Setm_TotalDegree(spolyrec *, sip_sring *)
  0.00     31.81     0.00     1176     0.00     0.00  sleftv::Typ(void)
  0.00     31.81     0.00     1124     0.00     0.00  omFreeBinPages
  0.00     31.81     0.00      988     0.00     0.00  sleftv::CleanUp(void)
  0.00     31.81     0.00      789     0.00     0.00  feReadLine(char *, int)
  0.00     31.81     0.00      789     0.00     0.00  yy_get_next_buffer(void)
  0.00     31.81     0.00      773     0.00     0.00  yy_get_previous_state(void)
  0.00     31.81     0.00      706     0.00     0.62  kBucketCanonicalize(kBucket *)
  0.00     31.81     0.00      610     0.00     0.00  pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
  0.00     31.81     0.00      575     0.00     0.00  sleftv::Data(void)
  0.00     31.81     0.00      560     0.00     0.00  iiCheckRing(int)
  0.00     31.81     0.00      538     0.00     0.00  enterOnePair(int, spolyrec *, int, int, skStrategy *, int)
  0.00     31.81     0.00      538     0.00     0.00  pHasNotCF(spolyrec *, spolyrec *)
  0.00     31.81     0.00      538     0.00     0.00  pLcm(spolyrec *, spolyrec *, spolyrec *)
  0.00     31.81     0.00      512     0.00     0.00  IsCmd(char *, int &)
  0.00     31.81     0.00      459     0.00     0.00  rIsPolyVar(int, sip_sring *)
  0.00     31.81     0.00      403     0.00     0.00  current_pos(int)
  0.00     31.81     0.00      392     0.00     0.00  kBucketInit(kBucket *, spolyrec *, int)
  0.00     31.81     0.00      378     0.00     0.00  fePrintEcho(char *, char *)
  0.00     31.81     0.00      354     0.00     0.62  kBucketClear(kBucket *, spolyrec **, int *)
  0.00     31.81     0.00      347     0.00     0.00  copy_string(lp_modes)
  0.00     31.81     0.00      346     0.00     0.00  sleftv::listLength(void)
  0.00     31.81     0.00      329     0.00     0.06  syMake(sleftv *, char *, idrec *)
  0.00     31.81     0.00      267     0.00     0.00  npInit(int)
  0.00     31.81     0.00      258     0.00     0.00  initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int)
  0.00     31.81     0.00      258     0.00     0.00  ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *)
  0.00     31.81     0.00      242     0.00     0.00  hStepS(long **, int, int *, int, int *, long *)
  0.00     31.81     0.00      239     0.00     0.00  iiTabIndex(sValCmdTab const *, int, int)
  0.00     31.81     0.00      204     0.00     0.00  kBucketCreate(sip_sring *)
  0.00     31.81     0.00      202     0.00     0.00  cancelunit(sLObject *)
  0.00     31.81     0.00      202     0.00     0.00  kBucketDestroy(kBucket **)
  0.00     31.81     0.00      196     0.00     0.00  p_LmCmp(spolyrec *, spolyrec *, sip_sring *)
  0.00     31.81     0.00      184     0.00     0.00  deleteInL(sLObject *, int *, int, skStrategy *)
  0.00     31.81     0.00      181     0.00     0.00  hHedge(spolyrec *)
  0.00     31.81     0.00      181     0.00     0.00  hPure(long **, int, int *, int *, int, long *, int *)
  0.00     31.81     0.00      174     0.00     0.00  hElimS(long **, int *, int, int, int *, int)
  0.00     31.81     0.00      174     0.00     0.00  hLex2S(long **, int, int, int, int *, int, long **)
  0.00     31.81     0.00      172     0.00     0.03  iiExprArith2(sleftv *, sleftv *, int, sleftv *, short)
  0.00     31.81     0.00      170     0.00     0.00  p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
  0.00     31.81     0.00      167     0.00     0.00  __builtin_vec_new
  0.00     31.81     0.00      164     0.00     0.80  enterT(sLObject, skStrategy *, int)
  0.00     31.81     0.00      158     0.00     0.00  posInT17(sTObject *, int, sLObject const &)
  0.00     31.81     0.00      156     0.00     0.00  Array<CanonicalForm>::operator[](int) const
  0.00     31.81     0.00      152     0.00     0.00  ndCopy(snumber *)
  0.00     31.81     0.00      133     0.00     0.01  sleftv::CopyD(int)
  0.00     31.81     0.00      122     0.00     0.00  enterid(char *, int, int, idrec **, short)
  0.00     31.81     0.00      122     0.00     0.00  idrec::set(char *, int, int, short)
  0.00     31.81     0.00      121     0.00     0.00  rIsRingVar(char *)
  0.00     31.81     0.00      111     0.00     0.00  jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00      106     0.00     0.03  iiAssign(sleftv *, sleftv *)
  0.00     31.81     0.00       97     0.00     0.00  yy_load_buffer_state(void)
  0.00     31.81     0.00       94     0.00     0.00  sleftv::LData(void)
  0.00     31.81     0.00       93     0.00     0.02  jiAssign_1(sleftv *, sleftv *)
  0.00     31.81     0.00       93     0.00     0.00  mALLOc
  0.00     31.81     0.00       90     0.00     0.00  jjPLUS_S(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       88     0.00     0.00  iiExprArithM(sleftv *, sleftv *, int)
  0.00     31.81     0.00       82     0.00     0.00  Array<CanonicalForm>::~Array(void)
  0.00     31.81     0.00       82     0.00     0.00  Array<CanonicalForm>::operator=(Array<CanonicalForm> const &)
  0.00     31.81     0.00       82     0.00     0.00  __builtin_vec_delete
  0.00     31.81     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(void)
  0.00     31.81     0.00       82     0.00     0.00  Array<CanonicalForm>::Array(int)
  0.00     31.81     0.00       79     0.00     0.00  iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short)
  0.00     31.81     0.00       79     0.00     0.00  iiTestConvert(int, int)
  0.00     31.81     0.00       76     0.00     0.00  ksCheckCoeff(snumber **, snumber **)
  0.00     31.81     0.00       76     0.00     0.12  ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **)
  0.00     31.81     0.00       76     0.00     0.00  ndGcd(snumber *, snumber *)
  0.00     31.81     0.00       74     0.00     0.00  iiConvert(int, int, int, sleftv *, sleftv *)
  0.00     31.81     0.00       71     0.00     0.00  malloc_usable_size
  0.00     31.81     0.00       70     0.00     0.00  omSizeOfLargeAddr
  0.00     31.81     0.00       69     0.00     0.00  fREe
  0.00     31.81     0.00       69     0.00     0.00  killhdl(idrec *, idrec **)
  0.00     31.81     0.00       68     0.00     0.00  hGetmem(int, long **, monrec *)
  0.00     31.81     0.00       68     0.00     0.00  hGetpure(long *)
  0.00     31.81     0.00       67     0.00   449.46  iiExprArith1(sleftv *, sleftv *, int)
  0.00     31.81     0.00       66     0.00     0.07  yy_flex_alloc(unsigned int)
  0.00     31.81     0.00       65     0.00     0.01  jiA_STRING(sleftv *, sleftv *, _ssubexpr *)
  0.00     31.81     0.00       65     0.00     0.00  yy_flush_buffer(yy_buffer_state *)
  0.00     31.81     0.00       65     0.00     0.00  yy_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     31.81     0.00       64     0.00     0.00  __builtin_new
  0.00     31.81     0.00       64     0.00     0.00  yy_flex_free(void *)
  0.00     31.81     0.00       63     0.00     0.00  __builtin_delete
  0.00     31.81     0.00       61     0.00     0.00  exitVoice(void)
  0.00     31.81     0.00       60     0.00     0.07  newBuffer(char *, feBufferTypes, procinfo *, int)
  0.00     31.81     0.00       50     0.00     0.00  npMult(snumber *, snumber *)
  0.00     31.81     0.00       49     0.00     0.00  npRead(char *, snumber **)
  0.00     31.81     0.00       49     0.00     0.22  pmInit(char *, short &)
  0.00     31.81     0.00       45     0.00     0.02  iiParameter(sleftv *)
  0.00     31.81     0.00       44     0.00     0.00  sleftv::String(void *, short, int)
  0.00     31.81     0.00       42     0.00     0.00  _omGetSpecBin
  0.00     31.81     0.00       41     0.00     0.00  iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short)
  0.00     31.81     0.00       41     0.00     0.00  omFreeSizeToSystem
  0.00     31.81     0.00       40     0.00     0.00  HEckeTest(spolyrec *, skStrategy *)
  0.00     31.81     0.00       40     0.00     0.01  chainCrit(spolyrec *, int, skStrategy *)
  0.00     31.81     0.00       40     0.00     0.00  enterSBba(sLObject, int, skStrategy *, int)
  0.00     31.81     0.00       40     0.00    13.31  enterSMora(sLObject, int, skStrategy *, int)
  0.00     31.81     0.00       40     0.00     0.03  enterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     31.81     0.00       40     0.00     0.03  initenterpairs(spolyrec *, int, int, int, skStrategy *, int)
  0.00     31.81     0.00       40     0.00     0.00  p_IsPurePower(spolyrec *, sip_sring *)
  0.00     31.81     0.00       40     0.00     0.00  redtail(spolyrec *, int, skStrategy *)
  0.00     31.81     0.00       39     0.00     0.00  posInS(spolyrec **, int, spolyrec *)
  0.00     31.81     0.00       36     0.00     0.01  malloc
  0.00     31.81     0.00       36     0.00     0.00  omReallocSizeFromSystem
  0.00     31.81     0.00       36     0.00     0.00  rEALLOc
  0.00     31.81     0.00       34     0.00     0.00  free
  0.00     31.81     0.00       33     0.00     0.00  sLObject::GetpLength(void)
  0.00     31.81     0.00       33     0.00     0.00  feCleanUpFile(char *)
  0.00     31.81     0.00       33     0.00     0.00  jjSTRING_PL(sleftv *, sleftv *)
  0.00     31.81     0.00       33     0.00     0.00  namerec::push(sip_package *, char *, int, short)
  0.00     31.81     0.00       32     0.00     0.00  sleftv::Copy(sleftv *)
  0.00     31.81     0.00       32     0.00     0.00  exitBuffer(feBufferTypes)
  0.00     31.81     0.00       32     0.00     0.00  iiCheckNest(void)
  0.00     31.81     0.00       32     0.00     0.00  iiConvName(char *)
  0.00     31.81     0.00       32     0.00     0.00  iiMake_proc(idrec *, sleftv *)
  0.00     31.81     0.00       32     0.00     0.07  iiPStart(idrec *, sleftv *)
  0.00     31.81     0.00       32     0.00     0.00  ipMoveId(idrec *)
  0.00     31.81     0.00       32     0.00     0.00  killlocals(int)
  0.00     31.81     0.00       32     0.00     0.13  myynewbuffer(void)
  0.00     31.81     0.00       32     0.00     0.00  myyoldbuffer(void *)
  0.00     31.81     0.00       32     0.00     0.00  npIsZero(snumber *)
  0.00     31.81     0.00       32     0.00     0.00  namerec::pop(short)
  0.00     31.81     0.00       32     0.00     0.00  yyrestart(_IO_FILE *)
  0.00     31.81     0.00       26     0.00     0.00  sleftv::Print(sleftv *, int)
  0.00     31.81     0.00       26     0.00     0.00  _omVallocFromSystem
  0.00     31.81     0.00       26     0.00     0.00  myfread(void *, unsigned int, unsigned int, _IO_FILE *)
  0.00     31.81     0.00       26     0.00     0.14  omAllocNewBinPagesRegion
  0.00     31.81     0.00       26     0.00     0.01  omBinPageIndexFault
  0.00     31.81     0.00       26     0.00     0.01  omRegisterBinPages
  0.00     31.81     0.00       25     0.00     0.00  jiA_INT(sleftv *, sleftv *, _ssubexpr *)
  0.00     31.81     0.00       25     0.00     0.00  jjSYSTEM(sleftv *, sleftv *)
  0.00     31.81     0.00       20     0.00     0.00  feGetResourceConfig(char)
  0.00     31.81     0.00       20     0.00     0.00  feResource(feResourceConfig_s *, int)
  0.00     31.81     0.00       18     0.00     0.00  iiS2Link(void *)
  0.00     31.81     0.00       18     0.00     0.00  jjCOUNT_L(sleftv *, sleftv *)
  0.00     31.81     0.00       18     0.00     0.00  jjGT_I(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       18     0.00     0.00  jjKLAMMER_PL(sleftv *, sleftv *)
  0.00     31.81     0.00       18     0.00     0.00  jjPROC(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       18     0.00     0.00  slCleanUp(sip_link *)
  0.00     31.81     0.00       18     0.00     0.00  slCloseAscii(sip_link *)
  0.00     31.81     0.00       18     0.00     0.00  slInit(sip_link *, char *)
  0.00     31.81     0.00       18     0.00     0.00  slKill(sip_link *)
  0.00     31.81     0.00       18     0.00     0.00  slOpenAscii(sip_link *, short)
  0.00     31.81     0.00       17     0.00     0.00  npEqual(snumber *, snumber *)
  0.00     31.81     0.00       16     0.00     0.00  jjDEFINED(sleftv *, sleftv *)
  0.00     31.81     0.00       15     0.00     0.00  p_ISet(int, sip_sring *)
  0.00     31.81     0.00       14     0.00     0.00  FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &)
  0.00     31.81     0.00       14     0.00     0.00  _omFindInSortedList
  0.00     31.81     0.00       14     0.00     0.00  jjMINUS_I(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       14     0.00     0.00  jjOP_REST(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       14     0.00     0.19  jjPOWER_P(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00       14     0.00     0.00  jjPROC1(sleftv *, sleftv *)
  0.00     31.81     0.00       14     0.00     0.17  pPower(spolyrec *, int)
  0.00     31.81     0.00       13     0.00     0.00  feFopen(char *, char *, char *, int, int)
  0.00     31.81     0.00       12     0.00     0.00  jjA_L_LIST(sleftv *, sleftv *)
  0.00     31.81     0.00       12     0.00     0.01  omDoRealloc
  0.00     31.81     0.00       11     0.00     0.00  iiWRITE(sleftv *, sleftv *)
  0.00     31.81     0.00       11     0.00     0.00  slWriteAscii(sip_link *, sleftv *)
  0.00     31.81     0.00       11     0.00     0.00  slWrite(sip_link *, sleftv *)
  0.00     31.81     0.00       10     0.00     0.00  malloc_extend_top
  0.00     31.81     0.00       10     0.00     0.20  slInternalCopy(sleftv *, int, void *, _ssubexpr *)
  0.00     31.81     0.00        9     0.00     0.00  lRingDependend(slists *)
  0.00     31.81     0.00        9     0.00     0.00  libread(_IO_FILE *, char *, int)
  0.00     31.81     0.00        9     0.00     0.00  omRealloc0Large
  0.00     31.81     0.00        9     0.00     0.00  p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
  0.00     31.81     0.00        9     0.00     0.00  yy_get_next_buffer(void)
  0.00     31.81     0.00        8     0.00     0.00  feResource(char, int)
  0.00     31.81     0.00        8     0.00     0.00  idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *)
  0.00     31.81     0.00        7     0.00     0.00  sleftv::Eval(void)
  0.00     31.81     0.00        7     0.00    34.16  firstUpdate(skStrategy *)
  0.00     31.81     0.00        7     0.00     0.00  hCreate(int)
  0.00     31.81     0.00        7     0.00     0.00  hDelete(long **, int)
  0.00     31.81     0.00        7     0.00     0.00  hHedgeStep(long *, long **, int, int *, int, spolyrec *)
  0.00     31.81     0.00        7     0.00     0.00  hInit(sip_sideal *, sip_sideal *, int *, sip_sring *)
  0.00     31.81     0.00        7     0.00     0.00  hKill(monrec **, int)
  0.00     31.81     0.00        7     0.00     0.00  hLexS(long **, int, int *, int)
  0.00     31.81     0.00        7     0.00     0.00  hOrdSupp(long **, int, int *, int)
  0.00     31.81     0.00        7     0.00     0.00  hStaircase(long **, int *, int *, int)
  0.00     31.81     0.00        7     0.00     0.00  iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.00  jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.17  jjPLUS_P(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.00  jjREAD(sleftv *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.25  jjTIMES_P(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.25  newHEdge(spolyrec **, int, skStrategy *)
  0.00     31.81     0.00        7     0.00     0.07  reorderL(skStrategy *)
  0.00     31.81     0.00        7     0.00     0.00  scComputeHC(sip_sideal *, int, spolyrec *&)
  0.00     31.81     0.00        7     0.00     0.00  slReadAscii2(sip_link *, sleftv *)
  0.00     31.81     0.00        7     0.00     0.00  slReadAscii(sip_link *)
  0.00     31.81     0.00        7     0.00     0.00  slRead(sip_link *, sleftv *)
  0.00     31.81     0.00        7     0.00    41.57  updateLHC(skStrategy *)
  0.00     31.81     0.00        7     0.00     0.00  yy_get_previous_state(void)
  0.00     31.81     0.00        6     0.00     0.00  _omInsertInSortedList
  0.00     31.81     0.00        6     0.00     0.00  feCleanResourceValue(feResourceType, char *)
  0.00     31.81     0.00        6     0.00     0.00  feInitResource(feResourceConfig_s *, int)
  0.00     31.81     0.00        6     0.00     0.00  feVerifyResourceValue(feResourceType, char *)
  0.00     31.81     0.00        6     0.00     0.00  idInit(int, int)
  0.00     31.81     0.00        6     0.00     0.00  posInT2(sTObject *, int, sLObject const &)
  0.00     31.81     0.00        5     0.00     0.00  iiGetLibProcBuffer(procinfo *, int)
  0.00     31.81     0.00        5     0.00     0.00  iiProcArgs(char *, short)
  0.00     31.81     0.00        4     0.00     0.00  feSprintf(char *, char const *, int)
  0.00     31.81     0.00        4     0.00     0.00  idrec::get(char const *, int)
  0.00     31.81     0.00        4     0.00     0.45  id_Delete(sip_sideal **, sip_sring *)
  0.00     31.81     0.00        4     0.00     0.61  initEcartNormal(sLObject *)
  0.00     31.81     0.00        4     0.00     0.00  jjINDEX_I(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        4     0.00     0.00  jjMakeSub(sleftv *)
  0.00     31.81     0.00        4     0.00     0.00  make_version(char *, int)
  0.00     31.81     0.00        4     0.00     0.00  mystrcpy(char *, char *)
  0.00     31.81     0.00        4     0.00     0.07  yy_flex_alloc(unsigned int)
  0.00     31.81     0.00        4     0.00     0.00  yy_flex_free(void *)
  0.00     31.81     0.00        4     0.00     0.00  yylp_flush_buffer(yy_buffer_state *)
  0.00     31.81     0.00        4     0.00     0.00  yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
  0.00     31.81     0.00        4     0.00     0.00  yylp_load_buffer_state(void)
  0.00     31.81     0.00        3     0.00     0.00  calloc
  0.00     31.81     0.00        3     0.00     3.33  jjMEMORY(sleftv *, sleftv *)
  0.00     31.81     0.00        3     0.00     0.00  malloc_update_mallinfo
  0.00     31.81     0.00        3     0.00     3.33  omGetUsedBinBytes
  0.00     31.81     0.00        3     0.00     3.33  omUpdateInfo
  0.00     31.81     0.00        3     0.00     0.00  pDiff(spolyrec *, int)
  0.00     31.81     0.00        2     0.00     0.00  ZeroOrd_2_NonZeroOrd(p_Ord, int)
  0.00     31.81     0.00        2     0.00     0.00  feCleanUpPath(char *)
  0.00     31.81     0.00        2     0.00     0.00  feGetOptIndex(char const *)
  0.00     31.81     0.00        2     0.00     0.00  fe_reset_fe
  0.00     31.81     0.00        2     0.00     0.00  fe_reset_input_mode(void)
  0.00     31.81     0.00        2     0.00     0.00  freeSize
  0.00     31.81     0.00        2     0.00     0.00  getTimer(void)
  0.00     31.81     0.00        2     0.00     5.18  iiLibCmd(char *, short)
  0.00     31.81     0.00        2     0.00     5.17  iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short)
  0.00     31.81     0.00        2     0.00     0.13  initL(void)
  0.00     31.81     0.00        2     0.00     0.46  jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *)
  0.00     31.81     0.00        2     0.00     0.00  kMoraUseBucket(skStrategy *)
  0.00     31.81     0.00        2     0.00     0.00  mmInit(void)
  0.00     31.81     0.00        2     0.00     0.00  newFile(char *, _IO_FILE *)
  0.00     31.81     0.00        2     0.00     0.00  omSizeOfAddr
  0.00     31.81     0.00        2     0.00     0.00  pEnlargeSet(spolyrec ***, int, int)
  0.00     31.81     0.00        2     0.00     0.00  reinit_yylp(void)
  0.00     31.81     0.00        2     0.00     0.13  yylp_create_buffer(_IO_FILE *, int)
  0.00     31.81     0.00        2     0.00     0.00  yylprestart(_IO_FILE *)
  0.00     31.81     0.00        2     0.00     0.00  yylpwrap
  0.00     31.81     0.00        1     0.00     0.00  IsPrime(int)
  0.00     31.81     0.00        1     0.00     0.00  PrintLn
  0.00     31.81     0.00        1     0.00     0.00  SetProcs(p_Field, p_Length, p_Ord)
  0.00     31.81     0.00        1     0.00     0.00  global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     31.81     0.00        1     0.00     0.00  global destructors keyed to gmp_output_digits
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to MP_INT_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to sattr::Print(void)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to fglmVector::fglmVector(fglmVectorRep *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to idealFunctionals::idealFunctionals(int, int)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to convSingNClapN(snumber *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to dArith2
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to feOptSpec
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to feVersionId
  0.00     31.81     0.00        1     0.00     0.13  global constructors keyed to fe_promptstr
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to fglmUpdatesource(sip_sideal *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to fglmcomb.cc
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to gmp_output_digits
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to iiCurrArgs
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to ip_smatrix_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to jjSYSTEM(sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to kBucketCreate(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to mpsr_sleftv_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to omSingOutOfMemoryFunc
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to rnumber_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to sSubexpr_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to s_si_link_extension_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to singclap_gcd(spolyrec *, spolyrec *)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to sip_command_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to slists_bin
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &)
  0.00     31.81     0.00        1     0.00     0.00  global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &)
  0.00     31.81     0.00        1     0.00     0.00  _fe_getopt_internal
  0.00     31.81     0.00        1     0.00     0.00  _omIsOnList
  0.00     31.81     0.00        1     0.00     0.00  _omRemoveFromList
  0.00     31.81     0.00        1     0.00     0.47  _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  atGet(sleftv *, char *)
  0.00     31.81     0.00        1     0.00    30.91  cleanT(skStrategy *)
  0.00     31.81     0.00        1     0.00    30.91  exitBuchMora(skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  exprlist_length(sleftv *)
  0.00     31.81     0.00        1     0.00     0.00  feGetExpandedExecutable(void)
  0.00     31.81     0.00        1     0.00     0.00  feInitResources(char *)
  0.00     31.81     0.00        1     0.00     0.00  feOptAction(feOptIndex)
  0.00     31.81     0.00        1     0.00     0.00  feSetOptValue(feOptIndex, int)
  0.00     31.81     0.00        1     0.00     0.00  fe_getopt_long
  0.00     31.81     0.00        1     0.00     0.00  full_readlink
  0.00     31.81     0.00        1     0.00     1.47  idCopy(sip_sideal *)
  0.00     31.81     0.00        1     0.00     0.00  idHomIdeal(sip_sideal *, sip_sideal *)
  0.00     31.81     0.00        1     0.00     0.00  idSkipZeroes(sip_sideal *)
  0.00     31.81     0.00        1     0.00     0.00  iiDummy(void *)
  0.00     31.81     0.00        1     0.00     0.00  iiExport(sleftv *, int)
  0.00     31.81     0.00        1     0.00     0.00  iiI2P(void *)
  0.00     31.81     0.00        1     0.00     0.00  initBuchMoraCrit(skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  initBuchMoraPos(skStrategy *)
  0.00     31.81     0.00        1     0.00    18.04  initBuchMora(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  initMora(sip_sideal *, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  initRTimer(void)
  0.00     31.81     0.00        1     0.00    17.78  initSL(sip_sideal *, sip_sideal *, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  initTimer(void)
  0.00     31.81     0.00        1     0.00     0.00  init_signals(void)
  0.00     31.81     0.00        1     0.00     0.00  inits(void)
  0.00     31.81     0.00        1     0.00     0.01  jiA_POLY(sleftv *, sleftv *, _ssubexpr *)
  0.00     31.81     0.00        1     0.00     0.00  jjInitTab1(void)
  0.00     31.81     0.00        1     0.00     0.00  jjJACOB_P(sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00     0.00  jjOPTION_PL(sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00 30103.99  jjSTD(sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00 30103.99  kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *)
  0.00     31.81     0.00        1     0.00     0.00  m2_end
  0.00     31.81     0.00        1     0.00 30199.31  main
  0.00     31.81     0.00        1     0.00     0.08  messageStat(int, int, int, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  mpsr_Init(void)
  0.00     31.81     0.00        1     0.00    10.26  nInitChar(sip_sring *)
  0.00     31.81     0.00        1     0.00    10.26  nSetChar(sip_sring *, short)
  0.00     31.81     0.00        1     0.00     0.00  npAdd(snumber *, snumber *)
  0.00     31.81     0.00        1     0.00     0.00  omFindExec
  0.00     31.81     0.00        1     0.00     0.00  omFindExec_link
  0.00     31.81     0.00        1     0.00     0.00  omFreeKeptAddrFromBin
  0.00     31.81     0.00        1     0.00     0.00  omGetStickyBinOfBin
  0.00     31.81     0.00        1     0.00     0.00  omInitInfo
  0.00     31.81     0.00        1     0.00     0.00  omIsKnownTopBin
  0.00     31.81     0.00        1     0.00     0.00  omMergeStickyBinIntoBin
  0.00     31.81     0.00        1     0.00     0.00  omSizeWOfAddr
  0.00     31.81     0.00        1     0.00     0.00  pIsConstant(spolyrec *)
  0.00     31.81     0.00        1     0.00     0.00  pIsHomogeneous(spolyrec *)
  0.00     31.81     0.00        1     0.00     0.00  pNormalize(spolyrec *)
  0.00     31.81     0.00        1     0.00     0.00  pSetGlobals(sip_sring *, short)
  0.00     31.81     0.00        1     0.00     0.00  p_FieldIs(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  p_GetSetmProc(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  p_OrdIs(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  p_SetProcs(sip_sring *, p_Procs_s *)
  0.00     31.81     0.00        1     0.00     0.00  pairs(skStrategy *)
  0.00     31.81     0.00        1     0.00    10.27  rComplete(sip_sring *, int)
  0.00     31.81     0.00        1     0.00     0.00  rFieldType(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  rGetDivMask(int)
  0.00     31.81     0.00        1     0.00     0.00  rGetExpSize(unsigned long, int &, int)
  0.00     31.81     0.00        1     0.00    13.60  rInit(sleftv *, sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00     0.00  rOrd_is_Totaldegree_Ordering(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  rOrderName(char *)
  0.00     31.81     0.00        1     0.00     0.00  rRightAdjustVarOffset(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  rSetDegStuff(sip_sring *)
  0.00     31.81     0.00        1     0.00    16.93  rSetHdl(idrec *, short)
  0.00     31.81     0.00        1     0.00     0.00  rSetNegWeight(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  rSetVarL(sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  rSleftvList2StringArray(sleftv *, char **)
  0.00     31.81     0.00        1     0.00     0.00  rSleftvOrdering2Ordering(sleftv *, sip_sring *)
  0.00     31.81     0.00        1     0.00     0.00  reorderS(int *, skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  reorderT(skStrategy *)
  0.00     31.81     0.00        1     0.00     0.00  setOption(sleftv *, sleftv *)
  0.00     31.81     0.00        1     0.00     0.00  slInitDBMExtension(s_si_link_extension *)
  0.00     31.81     0.00        1     0.00     0.00  slInitMPFileExtension(s_si_link_extension *)
  0.00     31.81     0.00        1     0.00     0.00  slInitMPTcpExtension(s_si_link_extension *)
  0.00     31.81     0.00        1     0.00     0.00  slStandardInit(void)
  0.00     31.81     0.00        1     0.00     0.00  updateS(short, skStrategy *)
  0.00     31.81     0.00        1     0.00   239.12  updateT(skStrategy *)
  0.00     31.81     0.00        1     0.00     0.13  yy_create_buffer(_IO_FILE *, int)

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


granularity: each sample hit covers 4 byte(s) for 0.03% of 30.67 seconds

index % time    self  children    called     name
                0.00   30.20       1/1           _start [2]
[1]     98.5    0.00   30.20       1         main [1]
                0.01   30.18       1/1           yyparse(void) <cycle 1> [39]
                0.00    0.01       1/2           iiLibCmd(char *, short) [59]
                0.00    0.00       1/118         Print [73]
                0.00    0.00       4/13          feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       2/2           newFile(char *, _IO_FILE *) [145]
                0.00    0.00       1/1           slStandardInit(void) [152]
                0.00    0.00       1/33          namerec::push(sip_package *, char *, int, short) [150]
                0.00    0.00       1/1           jjInitTab1(void) [336]
                0.00    0.00       1/1           inits(void) [335]
                0.00    0.00       1/1           feInitResources(char *) [319]
                0.00    0.00       1/1           fe_getopt_long [322]
-----------------------------------------------
                                                 <spontaneous>
[2]     98.5    0.00   30.20                 _start [2]
                0.00   30.20       1/1           main [1]
-----------------------------------------------
[3]     98.4    0.01   30.18       1+473     <cycle 1 as a whole> [3]
                0.00   30.11      67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.01    0.06      33             yyparse(void) <cycle 1> [39]
                0.00    0.01     172             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                0.00    0.00      32             iiPStart(idrec *, sleftv *) <cycle 1> [92]
                0.00    0.00      88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
                0.00    0.00      32             iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
                0.00    0.00      18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [241]
                0.00    0.00      18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [240]
-----------------------------------------------
                                  67             yyparse(void) <cycle 1> [39]
[4]     98.2    0.00   30.11      67         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00   30.10       1/1           jjSTD(sleftv *, sleftv *) [5]
                0.00    0.01       3/3           jjMEMORY(sleftv *, sleftv *) [74]
                0.00    0.00      23/74          iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00       1/1           jjJACOB_P(sleftv *, sleftv *) [154]
                0.00    0.00      90/988         sleftv::CleanUp(void) [163]
                0.00    0.00      67/1176        sleftv::Typ(void) [161]
                0.00    0.00      67/239         iiTabIndex(sValCmdTab const *, int, int) [179]
                0.00    0.00      23/79          iiTestConvert(int, int) [195]
                0.00    0.00      18/18          jjCOUNT_L(sleftv *, sleftv *) [238]
                0.00    0.00      16/16          jjDEFINED(sleftv *, sleftv *) [248]
                0.00    0.00       7/575         sleftv::Data(void) [166]
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [276]
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [253]
-----------------------------------------------
                0.00   30.10       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[5]     98.2    0.00   30.10       1         jjSTD(sleftv *, sleftv *) [5]
                0.00   30.10       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/1           atGet(sleftv *, char *) [316]
                0.00    0.00       1/575         sleftv::Data(void) [166]
                0.00    0.00       1/1           idSkipZeroes(sip_sideal *) [324]
-----------------------------------------------
                0.00   30.10       1/1           jjSTD(sleftv *, sleftv *) [5]
[6]     98.2    0.00   30.10       1         kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.04   30.06       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       1/1           idHomIdeal(sip_sideal *, sip_sideal *) [155]
                0.00    0.00       1/64          __builtin_new [4310]
                0.00    0.00       1/33          sLObject::GetpLength(void) [220]
                0.00    0.00       1/1           omGetStickyBinOfBin [344]
                0.00    0.00       1/8           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [264]
                0.00    0.00       1/1           omMergeStickyBinIntoBin [347]
                0.00    0.00       1/63          __builtin_delete [4311]
-----------------------------------------------
                0.04   30.06       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[7]     98.2    0.04   30.06       1         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.34   25.03   10212/10212       redFirst(sLObject *, skStrategy *) [10]
                0.10    3.85     432/432         redEcart(sLObject *, skStrategy *) [13]
                0.00    0.53      40/40          enterSMora(sLObject, int, skStrategy *, int) [18]
                0.05    0.02      37/161         pNorm(spolyrec *) [25]
                0.00    0.03      40/164         enterT(sLObject, skStrategy *, int) [33]
                0.00    0.03       1/1           exitBuchMora(skStrategy *) [43]
                0.01    0.02   10644/10644       message(int, int *, int *, skStrategy *) [48]
                0.00    0.02      40/354         kBucketClear(kBucket *, spolyrec **, int *) [30]
                0.00    0.02       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
                0.00    0.01      48/76          ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00      40/40          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
                0.00    0.00       2/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00       1/1           messageStat(int, int, int, skStrategy *) [129]
                0.00    0.00       4/130473      pLength(spolyrec *) [16]
                0.00    0.00      40/10655       PrintS [69]
                0.00    0.00       9/236720      omFreeToPageFault [21]
                0.00    0.00      40/202         kBucketDestroy(kBucket **) [143]
                0.00    0.00       4/204         kBucketCreate(sip_sring *) [147]
                0.00    0.00      40/40          redtail(spolyrec *, int, skStrategy *) [215]
                0.00    0.00      40/202         cancelunit(sLObject *) [180]
                0.00    0.00      39/39          posInS(spolyrec **, int, spolyrec *) [216]
                0.00    0.00       4/33          sLObject::GetpLength(void) [220]
                0.00    0.00       4/392         kBucketInit(kBucket *, spolyrec *, int) [173]
                0.00    0.00       1/1           initBuchMoraCrit(skStrategy *) [328]
                0.00    0.00       1/1           initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [330]
                0.00    0.00       1/1           initMora(sip_sideal *, skStrategy *) [331]
                0.00    0.00       1/1           initBuchMoraPos(skStrategy *) [329]
                0.00    0.00       1/2           kMoraUseBucket(skStrategy *) [306]
-----------------------------------------------
                0.01    3.06   28191/250155      doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.12   24.07  221964/250155      redFirst(sLObject *, skStrategy *) [10]
[8]     88.9    0.13   27.13  250155         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.16   26.48  250155/250203      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.36    0.04  250155/250203      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [22]
                0.05    0.00   28953/236720      omFreeToPageFault [21]
                0.03    0.00  250155/440834      npIsOne(snumber *) [41]
                0.02    0.00  250155/441132      nDummy1(snumber **) [45]
                0.00    0.00      38/202         kBucketDestroy(kBucket **) [143]
-----------------------------------------------
                0.00    0.01      48/250203      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.16   26.48  250155/250203      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[9]     86.9    0.16   26.48  250203         kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
               18.49    2.05  171046/171074      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
                4.72    0.28   36755/39961       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [12]
                0.38    0.49   72696/215976      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                0.04    0.00    6461/23744       pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.03    0.00  158314/158314      npNeg(snumber *) [47]
-----------------------------------------------
                0.34   25.03   10212/10212       mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[10]    82.7    0.34   25.03   10212         redFirst(sLObject *, skStrategy *) [10]
                0.12   24.07  221964/250155      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.46    0.00  221970/250205      kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [19]
                0.22    0.00  232140/260805      p_GetShortExpVector(spolyrec *, sip_sring *) [27]
                0.08    0.00  221934/251095      pDeg(spolyrec *, sip_sring *) [38]
                0.05    0.00   23962/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
                0.01    0.00   10170/10556       kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [70]
                0.01    0.00   10170/10932       enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00       6/706         kBucketCanonicalize(kBucket *) [20]
                0.00    0.00       6/198         pLDeg0c(spolyrec *, int *, sip_sring *) [36]
                0.00    0.00      14/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00      28/171074      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
               18.49    2.05  171046/171074      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[11]    67.0   18.49    2.05  171074         p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
                0.75    0.97  143252/215976      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                0.11    0.09  133208/237049      omAllocBinFromFullPage [23]
                0.10    0.01   17232/23744       pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.02    0.00   12147/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00       7/39961       jjPLUS_P(sleftv *, sleftv *, sleftv *) [102]
                0.41    0.02    3199/39961       kBucketCanonicalize(kBucket *) [20]
                4.72    0.28   36755/39961       kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[12]    17.7    5.13    0.31   39961         p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [12]
                0.31    0.00  169132/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.10    3.85     432/432         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[13]    12.9    0.10    3.85     432         redEcart(sLObject *, skStrategy *) [13]
                0.02    3.67   28191/28191       doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.06    0.00   28235/250205      kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [19]
                0.05    0.00   20324/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
                0.03    0.00   28621/260805      p_GetShortExpVector(spolyrec *, sip_sring *) [27]
                0.00    0.02      34/706         kBucketCanonicalize(kBucket *) [20]
                0.01    0.00   28189/251095      pDeg(spolyrec *, sip_sring *) [38]
                0.00    0.00     386/10556       kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [70]
                0.00    0.00     396/10932       enterL(sLObject **, int *, int *, sLObject, int) [67]
-----------------------------------------------
                0.02    3.67   28191/28191       redEcart(sLObject *, skStrategy *) [13]
[14]    12.0    0.02    3.67   28191         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.01    3.06   28191/250155      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.16    0.06     120/161         pNorm(spolyrec *) [25]
                0.11    0.01     248/259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.00    0.10     124/164         enterT(sLObject, skStrategy *, int) [33]
                0.00    0.08     124/706         kBucketCanonicalize(kBucket *) [20]
                0.00    0.08     124/354         kBucketClear(kBucket *, spolyrec **, int *) [30]
                0.00    0.00     124/202         kBucketDestroy(kBucket **) [143]
                0.00    0.00     124/204         kBucketCreate(sip_sring *) [147]
                0.00    0.00     124/392         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.00      28/215976      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.38    0.49   72696/215976      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.75    0.97  143252/215976      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
[15]     8.4    1.13    1.46  215976         pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                1.32    0.00  130469/130473      pLength(spolyrec *) [16]
                0.07    0.06   88272/237049      omAllocBinFromFullPage [23]
                0.00    0.00      21/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00       4/130473      mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                1.32    0.00  130469/130473      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
[16]     4.3    1.32    0.00  130473         pLength(spolyrec *) [16]
-----------------------------------------------
                0.00    0.00       4/352         initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.08    0.16     158/352         updateT(skStrategy *) [29]
                0.10    0.19     190/352         updateLHC(skStrategy *) [26]
[17]     1.7    0.18    0.35     352         deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.12     190/354         kBucketClear(kBucket *, spolyrec **, int *) [30]
                0.00    0.12     188/706         kBucketCanonicalize(kBucket *) [20]
                0.11    0.00     188/198         pLDeg0c(spolyrec *, int *, sip_sring *) [36]
                0.01    0.00      25/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00     188/251095      pDeg(spolyrec *, sip_sring *) [38]
                0.00    0.00     190/196         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [181]
                0.00    0.00     188/392         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.53      40/40          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[18]     1.7    0.00    0.53      40         enterSMora(sLObject, int, skStrategy *, int) [18]
                0.00    0.29       7/7           updateLHC(skStrategy *) [26]
                0.00    0.24       7/7           firstUpdate(skStrategy *) [28]
                0.00    0.00       7/7           newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00       7/7           reorderL(skStrategy *) [108]
                0.00    0.00      40/40          enterSBba(sLObject, int, skStrategy *, int) [213]
                0.00    0.00      40/40          HEckeTest(spolyrec *, skStrategy *) [212]
-----------------------------------------------
                0.06    0.00   28235/250205      redEcart(sLObject *, skStrategy *) [13]
                0.46    0.00  221970/250205      redFirst(sLObject *, skStrategy *) [10]
[19]     1.7    0.52    0.00  250205         kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [19]
-----------------------------------------------
                0.00    0.00       6/706         redFirst(sLObject *, skStrategy *) [10]
                0.00    0.02      34/706         redEcart(sLObject *, skStrategy *) [13]
                0.00    0.08     124/706         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.00    0.12     188/706         deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.22     354/706         kBucketClear(kBucket *, spolyrec **, int *) [30]
[20]     1.4    0.00    0.44     706         kBucketCanonicalize(kBucket *) [20]
                0.41    0.02    3199/39961       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [12]
-----------------------------------------------
                0.00    0.00       1/236720      deleteInL(sLObject *, int *, int, skStrategy *) [146]
                0.00    0.00       1/236720      enterT(sLObject, skStrategy *, int) [33]
                0.00    0.00       1/236720      exitBuchMora(skStrategy *) [43]
                0.00    0.00       1/236720      rComplete(sip_sring *, int) [61]
                0.00    0.00       1/236720      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.00    0.00       2/236720      kBucketDestroy(kBucket **) [143]
                0.00    0.00       3/236720      scComputeHC(sip_sideal *, int, spolyrec *&) [142]
                0.00    0.00       5/236720      feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       6/236720      updateLHC(skStrategy *) [26]
                0.00    0.00       9/236720      mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      14/236720      redFirst(sLObject *, skStrategy *) [10]
                0.00    0.00      21/236720      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                0.00    0.00      54/236720      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00    2068/236720      p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.01    0.00    4760/236720      p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [34]
                0.02    0.00   12147/236720      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
                0.04    0.00   19541/236720      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [22]
                0.05    0.00   28953/236720      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.31    0.00  169132/236720      p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [12]
[21]     1.4    0.43    0.00  236720         omFreeToPageFault [21]
                0.00    0.00    1124/1124        omFreeBinPages [162]
-----------------------------------------------
                0.00    0.00      48/250203      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.36    0.04  250155/250203      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[22]     1.3    0.36    0.04  250203         p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [22]
                0.04    0.00   19541/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00       1/237049      idrec::set(char *, int, int, short) [149]
                0.00    0.00       1/237049      namerec::push(sip_package *, char *, int, short) [150]
                0.00    0.00       1/237049      iiCheckNest(void) [151]
                0.00    0.00       1/237049      rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.00       1/237049      rComplete(sip_sring *, int) [61]
                0.00    0.00       1/237049      slStandardInit(void) [152]
                0.00    0.00       1/237049      kBucketCreate(sip_sring *) [147]
                0.00    0.00       2/237049      iiGetLibProcBuffer(procinfo *, int) [141]
                0.00    0.00       2/237049      enterT(sLObject, skStrategy *, int) [33]
                0.00    0.00       3/237049      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.00    0.00       5/237049      feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00      18/237049      __builtin_vec_new [120]
                0.00    0.00    1156/237049      p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [34]
                0.00    0.00    4695/237049      p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.01    0.01    9681/237049      pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.07    0.06   88272/237049      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                0.11    0.09  133208/237049      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
[23]     1.2    0.20    0.16  237049         omAllocBinFromFullPage [23]
                0.16    0.00    3391/3391        omAllocBinPage [31]
-----------------------------------------------
                                                 <spontaneous>
[24]     1.1    0.33    0.00                 write [24]
-----------------------------------------------
                0.01    0.00       4/161         initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.05    0.02      37/161         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.16    0.06     120/161         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
[25]     1.0    0.21    0.08     161         pNorm(spolyrec *) [25]
                0.03    0.00  190279/190279      npDiv(snumber *, snumber *) [46]
                0.02    0.00  190440/440834      npIsOne(snumber *) [41]
                0.02    0.00  190418/190418      nDummy2(snumber *&) [50]
                0.01    0.00  190438/441132      nDummy1(snumber **) [45]
                0.00    0.00     159/267         npInit(int) [177]
-----------------------------------------------
                0.00    0.29       7/7           enterSMora(sLObject, int, skStrategy *, int) [18]
[26]     0.9    0.00    0.29       7         updateLHC(skStrategy *) [26]
                0.10    0.19     190/352         deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.00      28/76          ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00      56/251095      pDeg(spolyrec *, sip_sring *) [38]
                0.00    0.00       6/236720      omFreeToPageFault [21]
                0.00    0.00      28/204         kBucketCreate(sip_sring *) [147]
                0.00    0.00       3/184         deleteInL(sLObject *, int *, int, skStrategy *) [146]
                0.00    0.00      28/33          sLObject::GetpLength(void) [220]
                0.00    0.00      28/392         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.00       4/260805      initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00      40/260805      enterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
                0.03    0.00   28621/260805      redEcart(sLObject *, skStrategy *) [13]
                0.22    0.00  232140/260805      redFirst(sLObject *, skStrategy *) [10]
[27]     0.8    0.25    0.00  260805         p_GetShortExpVector(spolyrec *, sip_sring *) [27]
-----------------------------------------------
                0.00    0.24       7/7           enterSMora(sLObject, int, skStrategy *, int) [18]
[28]     0.8    0.00    0.24       7         firstUpdate(skStrategy *) [28]
                0.00    0.24       1/1           updateT(skStrategy *) [29]
                0.00    0.00       1/2           kMoraUseBucket(skStrategy *) [306]
                0.00    0.00       1/1           reorderT(skStrategy *) [369]
-----------------------------------------------
                0.00    0.24       1/1           firstUpdate(skStrategy *) [28]
[29]     0.8    0.00    0.24       1         updateT(skStrategy *) [29]
                0.08    0.16     158/352         deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.00     158/202         cancelunit(sLObject *) [180]
-----------------------------------------------
                0.00    0.02      40/354         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.08     124/354         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.00    0.12     190/354         deleteHC(sLObject *, skStrategy *, short) [17]
[30]     0.7    0.00    0.22     354         kBucketClear(kBucket *, spolyrec **, int *) [30]
                0.00    0.22     354/706         kBucketCanonicalize(kBucket *) [20]
-----------------------------------------------
                0.16    0.00    3391/3391        omAllocBinFromFullPage [23]
[31]     0.5    0.16    0.00    3391         omAllocBinPage [31]
                0.00    0.00      26/26          omAllocNewBinPagesRegion [87]
-----------------------------------------------
                0.00    0.00       3/23744       _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00      48/23744       ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.04    0.00    6461/23744       kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.10    0.01   17232/23744       p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
[32]     0.5    0.14    0.01   23744         pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.01    0.01    9681/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.03      40/164         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.10     124/164         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
[33]     0.4    0.00    0.13     164         enterT(sLObject, skStrategy *, int) [33]
                0.12    0.01     164/164         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [34]
                0.00    0.00      10/12          omDoRealloc [125]
                0.00    0.00       2/237049      omAllocBinFromFullPage [23]
                0.00    0.00       1/236720      omFreeToPageFault [21]
                0.00    0.00     158/158         posInT17(sTObject *, int, sLObject const &) [187]
                0.00    0.00       6/6           posInT2(sTObject *, int, sLObject const &) [285]
-----------------------------------------------
                0.12    0.01     164/164         enterT(sLObject, skStrategy *, int) [33]
[34]     0.4    0.12    0.01     164         p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s [34]
                0.01    0.00    4760/236720      omFreeToPageFault [21]
                0.00    0.00    1156/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       1/259         slInternalCopy(sleftv *, int, void *, _ssubexpr *) [94]
                0.00    0.00       3/259         idCopy(sip_sideal *) [99]
                0.00    0.00       3/259         pPower(spolyrec *, int) [91]
                0.00    0.00       4/259         initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.11    0.01     248/259         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
[35]     0.4    0.12    0.01     259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.00    0.00    4695/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       4/198         initEcartNormal(sLObject *) [90]
                0.00    0.00       6/198         redFirst(sLObject *, skStrategy *) [10]
                0.11    0.00     188/198         deleteHC(sLObject *, skStrategy *, short) [17]
[36]     0.4    0.12    0.00     198         pLDeg0c(spolyrec *, int *, sip_sring *) [36]
                0.00    0.00     198/251095      pDeg(spolyrec *, sip_sring *) [38]
-----------------------------------------------
                0.00    0.00       3/44860       initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00     104/44860       chainCrit(spolyrec *, int, skStrategy *) [111]
                0.00    0.00     209/44860       reorderL(skStrategy *) [108]
                0.00    0.00     258/44860       enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
                0.05    0.00   20324/44860       redEcart(sLObject *, skStrategy *) [13]
                0.05    0.00   23962/44860       redFirst(sLObject *, skStrategy *) [10]
[37]     0.3    0.10    0.00   44860         posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
-----------------------------------------------
                0.00    0.00       3/251095      pIsHomogeneous(spolyrec *) [156]
                0.00    0.00       4/251095      initEcartNormal(sLObject *) [90]
                0.00    0.00       7/251095      newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00      56/251095      updateLHC(skStrategy *) [26]
                0.00    0.00     188/251095      deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.00     198/251095      pLDeg0c(spolyrec *, int *, sip_sring *) [36]
                0.00    0.00     516/251095      initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [119]
                0.01    0.00   28189/251095      redEcart(sLObject *, skStrategy *) [13]
                0.08    0.00  221934/251095      redFirst(sLObject *, skStrategy *) [10]
[38]     0.3    0.09    0.00  251095         pDeg(spolyrec *, sip_sring *) [38]
-----------------------------------------------
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [92]
                0.01   30.18       1/1           main [1]
[39]     0.2    0.01    0.06      33         yyparse(void) <cycle 1> [39]
                0.00    0.02     329/329         syMake(sleftv *, char *, idrec *) [49]
                0.00    0.02       1/1           rSetHdl(idrec *, short) [56]
                0.00    0.01       1/1           rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.01       1/2           iiLibCmd(char *, short) [59]
                0.00    0.00      28/60          newBuffer(char *, feBufferTypes, procinfo *, int) [85]
                0.00    0.00      67/106         iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00      45/45          iiParameter(sleftv *) [103]
                0.00    0.00    1820/1820        yylex(MYYSTYPE *) [109]
                0.00    0.00      26/26          sleftv::Print(sleftv *, int) [128]
                0.00    0.00      79/79          iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [157]
                0.00    0.00       1/122         enterid(char *, int, int, idrec **, short) [148]
                0.00    0.00      76/1176        sleftv::Typ(void) [161]
                0.00    0.00      63/988         sleftv::CleanUp(void) [163]
                0.00    0.00      51/575         sleftv::Data(void) [166]
                0.00    0.00      32/32          exitBuffer(feBufferTypes) [224]
                0.00    0.00      20/32          sleftv::Copy(sleftv *) [223]
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [274]
                0.00    0.00       1/1           rOrderName(char *) [361]
                0.00    0.00       1/64          __builtin_new [4310]
                0.00    0.00       1/1           iiExport(sleftv *, int) [326]
                                 154             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                                  88             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
                                  67             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
-----------------------------------------------
                0.00    0.00       2/240         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00       2/240         pPower(spolyrec *, int) [91]
                0.00    0.00       2/240         _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00       7/240         jjTIMES_P(sleftv *, sleftv *, sleftv *) [97]
                0.00    0.00       7/240         newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00       8/240         id_Delete(sip_sideal **, sip_sring *) [96]
                0.01    0.00      25/240         deleteHC(sLObject *, skStrategy *, short) [17]
                0.01    0.00      49/240         pmInit(char *, short &) [58]
                0.03    0.00     138/240         cleanT(skStrategy *) [44]
[40]     0.2    0.05    0.00     240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00    2068/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00      11/440834      pPower(spolyrec *, int) [91]
                0.00    0.00     228/440834      ksCheckCoeff(snumber **, snumber **) [135]
                0.02    0.00  190440/440834      pNorm(spolyrec *) [25]
                0.03    0.00  250155/440834      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[41]     0.2    0.05    0.00  440834         npIsOne(snumber *) [41]
-----------------------------------------------
                                                 <spontaneous>
[42]     0.1    0.04    0.00                 _IO_file_xsputn [42]
-----------------------------------------------
                0.00    0.03       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[43]     0.1    0.00    0.03       1         exitBuchMora(skStrategy *) [43]
                0.00    0.03       1/1           cleanT(skStrategy *) [44]
                0.00    0.00       1/236720      omFreeToPageFault [21]
                0.00    0.00       1/441132      nDummy1(snumber **) [45]
                0.00    0.00       3/70          omSizeOfLargeAddr [198]
                0.00    0.00       3/41          omFreeSizeToSystem [211]
-----------------------------------------------
                0.00    0.03       1/1           exitBuchMora(skStrategy *) [43]
[44]     0.1    0.00    0.03       1         cleanT(skStrategy *) [44]
                0.03    0.00     138/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
-----------------------------------------------
                0.00    0.00       1/441132      exitBuchMora(skStrategy *) [43]
                0.00    0.00       2/441132      _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00      16/441132      pDiff(spolyrec *, int) [153]
                0.00    0.00      76/441132      ksCheckCoeff(snumber **, snumber **) [135]
                0.00    0.00     152/441132      ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00     292/441132      ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.01    0.00  190438/441132      pNorm(spolyrec *) [25]
                0.02    0.00  250155/441132      ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
[45]     0.1    0.03    0.00  441132         nDummy1(snumber **) [45]
-----------------------------------------------
                0.03    0.00  190279/190279      pNorm(spolyrec *) [25]
[46]     0.1    0.03    0.00  190279         npDiv(snumber *, snumber *) [46]
-----------------------------------------------
                0.03    0.00  158314/158314      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
[47]     0.1    0.03    0.00  158314         npNeg(snumber *) [47]
-----------------------------------------------
                0.01    0.02   10644/10644       mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[48]     0.1    0.01    0.02   10644         message(int, int *, int *, skStrategy *) [48]
                0.01    0.00   10615/10655       PrintS [69]
                0.01    0.00     112/118         Print [73]
-----------------------------------------------
                0.00    0.02     329/329         yyparse(void) <cycle 1> [39]
[49]     0.1    0.00    0.02     329         syMake(sleftv *, char *, idrec *) [49]
                0.00    0.01      49/49          pmInit(char *, short &) [58]
                0.01    0.00     329/329         ggetid(char const *, short) [71]
                0.00    0.00      23/121         rIsRingVar(char *) [189]
                0.00    0.00      14/15          p_ISet(int, sip_sring *) [249]
                0.00    0.00      14/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
-----------------------------------------------
                0.02    0.00  190418/190418      pNorm(spolyrec *) [25]
[50]     0.1    0.02    0.00  190418         nDummy2(snumber *&) [50]
-----------------------------------------------
                                                 <spontaneous>
[51]     0.1    0.02    0.00                 fork [51]
-----------------------------------------------
                                                 <spontaneous>
[52]     0.1    0.02    0.00                 fwrite [52]
-----------------------------------------------
                                                 <spontaneous>
[53]     0.1    0.02    0.00                 memmove [53]
-----------------------------------------------
                0.00    0.02       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[54]     0.1    0.00    0.02       1         initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
                0.00    0.02       1/1           initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00       2/2           initL(void) [117]
                0.00    0.00       1/1           updateS(short, skStrategy *) [374]
                0.00    0.00       1/1           pairs(skStrategy *) [356]
-----------------------------------------------
                0.00    0.02       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
[55]     0.1    0.00    0.02       1         initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.01    0.00       4/161         pNorm(spolyrec *) [25]
                0.00    0.00       4/352         deleteHC(sLObject *, skStrategy *, short) [17]
                0.00    0.00       4/4           initEcartNormal(sLObject *) [90]
                0.00    0.00       4/259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.00    0.00       3/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
                0.00    0.00       4/260805      p_GetShortExpVector(spolyrec *, sip_sring *) [27]
                0.00    0.00       4/10932       enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00       4/202         cancelunit(sLObject *) [180]
                0.00    0.00       1/6           idInit(int, int) [284]
                0.00    0.00       1/1           pIsConstant(spolyrec *) [349]
-----------------------------------------------
                0.00    0.02       1/1           yyparse(void) <cycle 1> [39]
[56]     0.1    0.00    0.02       1         rSetHdl(idrec *, short) [56]
                0.00    0.01       1/1           nSetChar(sip_sring *, short) [64]
                0.01    0.00       2/3           rDBTest(sip_sring *, char *, int) [77]
                0.00    0.00       1/1           pSetGlobals(sip_sring *, short) [351]
-----------------------------------------------
                0.00    0.01       1/1           yyparse(void) <cycle 1> [39]
[57]     0.0    0.00    0.01       1         rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.01       1/1           rComplete(sip_sring *, int) [61]
                0.00    0.00       1/3           rDBTest(sip_sring *, char *, int) [77]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
                0.00    0.00       2/988         sleftv::CleanUp(void) [163]
                0.00    0.00       1/1176        sleftv::Typ(void) [161]
                0.00    0.00       1/575         sleftv::Data(void) [166]
                0.00    0.00       1/1           IsPrime(int) [313]
                0.00    0.00       1/346         sleftv::listLength(void) [176]
                0.00    0.00       1/1           rSleftvList2StringArray(sleftv *, char **) [366]
                0.00    0.00       1/1           rSleftvOrdering2Ordering(sleftv *, sip_sring *) [367]
-----------------------------------------------
                0.00    0.01      49/49          syMake(sleftv *, char *, idrec *) [49]
[58]     0.0    0.00    0.01      49         pmInit(char *, short &) [58]
                0.01    0.00      49/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00      98/121         rIsRingVar(char *) [189]
                0.00    0.00      49/49          npRead(char *, snumber **) [208]
-----------------------------------------------
                0.00    0.01       1/2           main [1]
                0.00    0.01       1/2           yyparse(void) <cycle 1> [39]
[59]     0.0    0.00    0.01       2         iiLibCmd(char *, short) [59]
                0.00    0.01       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [60]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       1/122         enterid(char *, int, int, idrec **, short) [148]
                0.00    0.00       2/4           idrec::get(char const *, int) [288]
-----------------------------------------------
                0.00    0.01       2/2           iiLibCmd(char *, short) [59]
[60]     0.0    0.00    0.01       2         iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [60]
                0.01    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
                0.00    0.00       1/118         Print [73]
                0.00    0.00       2/2           reinit_yylp(void) [310]
-----------------------------------------------
                0.00    0.01       1/1           rInit(sleftv *, sleftv *, sleftv *) [57]
[61]     0.0    0.00    0.01       1         rComplete(sip_sring *, int) [61]
                0.00    0.01       1/1           nInitChar(sip_sring *) [63]
                0.00    0.00       1/236720      omFreeToPageFault [21]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
                0.00    0.00       1/1           omSizeWOfAddr [348]
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [363]
                0.00    0.00       1/1           rGetExpSize(unsigned long, int &, int) [359]
                0.00    0.00       1/1           rGetDivMask(int) [358]
                0.00    0.00       1/42          _omGetSpecBin [4312]
                0.00    0.00       1/1           p_GetSetmProc(sip_sring *) [353]
                0.00    0.00       1/1           rSetVarL(sip_sring *) [365]
                0.00    0.00       1/1           rRightAdjustVarOffset(sip_sring *) [362]
                0.00    0.00       1/1           rSetNegWeight(sip_sring *) [364]
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [355]
-----------------------------------------------
                0.01    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [60]
[62]     0.0    0.01    0.00       2         yylplex(char *, char *, lib_style_types *, lp_modes) [62]
                0.00    0.00       2/2           yylp_create_buffer(_IO_FILE *, int) [118]
                0.00    0.00      41/122         enterid(char *, int, int, idrec **, short) [148]
                0.00    0.00     403/403         current_pos(int) [172]
                0.00    0.00     347/347         copy_string(lp_modes) [175]
                0.00    0.00      41/41          iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [210]
                0.00    0.00       9/9           yy_get_next_buffer(void) [262]
                0.00    0.00       7/7           yy_get_previous_state(void) [280]
                0.00    0.00       4/4           make_version(char *, int) [291]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [296]
                0.00    0.00       2/2           yylpwrap [312]
-----------------------------------------------
                0.00    0.01       1/1           rComplete(sip_sring *, int) [61]
[63]     0.0    0.00    0.01       1         nInitChar(sip_sring *) [63]
                0.01    0.00       1/1           npInitChar(int, sip_sring *) [65]
                0.00    0.00       1/1           rFieldType(sip_sring *) [357]
                0.00    0.00       1/267         npInit(int) [177]
-----------------------------------------------
                0.00    0.01       1/1           rSetHdl(idrec *, short) [56]
[64]     0.0    0.00    0.01       1         nSetChar(sip_sring *, short) [64]
                0.01    0.00       1/1           npSetChar(int, sip_sring *) [66]
-----------------------------------------------
                0.01    0.00       1/1           nInitChar(sip_sring *) [63]
[65]     0.0    0.01    0.00       1         npInitChar(int, sip_sring *) [65]
                0.00    0.00       2/76          omAllocFromSystem [68]
-----------------------------------------------
                0.01    0.00       1/1           nSetChar(sip_sring *, short) [64]
[66]     0.0    0.01    0.00       1         npSetChar(int, sip_sring *) [66]
                0.00    0.00       2/76          omAllocFromSystem [68]
-----------------------------------------------
                0.00    0.00       4/10932       initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00     104/10932       chainCrit(spolyrec *, int, skStrategy *) [111]
                0.00    0.00     258/10932       enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
                0.00    0.00     396/10932       redEcart(sLObject *, skStrategy *) [13]
                0.01    0.00   10170/10932       redFirst(sLObject *, skStrategy *) [10]
[67]     0.0    0.01    0.00   10932         enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00       2/12          omDoRealloc [125]
-----------------------------------------------
                0.00    0.00       1/76          global constructors keyed to fe_promptstr [126]
                0.00    0.00       1/76          omDoRealloc [125]
                0.00    0.00       1/76          __builtin_vec_new [120]
                0.00    0.00       1/76          omBinPageIndexFault [122]
                0.00    0.00       2/76          feReadLine(char *, int) [114]
                0.00    0.00       2/76          initL(void) [117]
                0.00    0.00       2/76          npSetChar(int, sip_sring *) [66]
                0.00    0.00       2/76          npInitChar(int, sip_sring *) [65]
                0.00    0.00       2/76          yy_flex_alloc(unsigned int) [116]
                0.00    0.00       3/76          malloc [110]
                0.00    0.00      26/76          omAllocNewBinPagesRegion [87]
                0.00    0.00      33/76          yy_flex_alloc(unsigned int) [84]
[68]     0.0    0.01    0.00      76         omAllocFromSystem [68]
                0.00    0.00      76/93          mALLOc [193]
-----------------------------------------------
                0.00    0.00      40/10655       mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.01    0.00   10615/10655       message(int, int *, int *, skStrategy *) [48]
[69]     0.0    0.01    0.00   10655         PrintS [69]
-----------------------------------------------
                0.00    0.00     386/10556       redEcart(sLObject *, skStrategy *) [13]
                0.01    0.00   10170/10556       redFirst(sLObject *, skStrategy *) [10]
[70]     0.0    0.01    0.00   10556         kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [70]
-----------------------------------------------
                0.01    0.00     329/329         syMake(sleftv *, char *, idrec *) [49]
[71]     0.0    0.01    0.00     329         ggetid(char const *, short) [71]
-----------------------------------------------
                0.01    0.00     162/162         omGetUsedBinBytes [75]
[72]     0.0    0.01    0.00     162         omGetUsedBytesOfBin [72]
-----------------------------------------------
                0.00    0.00       1/118         main [1]
                0.00    0.00       1/118         iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [60]
                0.00    0.00       1/118         messageStat(int, int, int, skStrategy *) [129]
                0.00    0.00       1/118         sleftv::Print(sleftv *, int) [128]
                0.00    0.00       2/118         newHEdge(spolyrec **, int, skStrategy *) [98]
                0.01    0.00     112/118         message(int, int *, int *, skStrategy *) [48]
[73]     0.0    0.01    0.00     118         Print [73]
-----------------------------------------------
                0.00    0.01       3/3           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[74]     0.0    0.00    0.01       3         jjMEMORY(sleftv *, sleftv *) [74]
                0.00    0.01       3/3           omUpdateInfo [76]
                0.00    0.00       3/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.01       3/3           omUpdateInfo [76]
[75]     0.0    0.00    0.01       3         omGetUsedBinBytes [75]
                0.01    0.00     162/162         omGetUsedBytesOfBin [72]
-----------------------------------------------
                0.00    0.01       3/3           jjMEMORY(sleftv *, sleftv *) [74]
[76]     0.0    0.00    0.01       3         omUpdateInfo [76]
                0.00    0.01       3/3           omGetUsedBinBytes [75]
                0.00    0.00       3/3           malloc_update_mallinfo [298]
-----------------------------------------------
                0.00    0.00       1/3           rInit(sleftv *, sleftv *, sleftv *) [57]
                0.01    0.00       2/3           rSetHdl(idrec *, short) [56]
[77]     0.0    0.01    0.00       3         rDBTest(sip_sring *, char *, int) [77]
-----------------------------------------------
                                                 <spontaneous>
[78]     0.0    0.01    0.00                 _IO_do_write [78]
-----------------------------------------------
                                                 <spontaneous>
[79]     0.0    0.01    0.00                 _IO_file_sync [79]
-----------------------------------------------
                                                 <spontaneous>
[80]     0.0    0.01    0.00                 _IO_file_write [80]
-----------------------------------------------
                                                 <spontaneous>
[81]     0.0    0.01    0.00                 _IO_link_in [81]
-----------------------------------------------
                0.00    0.00      28/76          updateLHC(skStrategy *) [26]
                0.00    0.01      48/76          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[82]     0.0    0.00    0.01      76         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.01      48/250203      kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [9]
                0.00    0.00      28/171074      p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec [11]
                0.00    0.00      28/215976      pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec [15]
                0.00    0.00      48/23744       pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.00    0.00      54/236720      omFreeToPageFault [21]
                0.00    0.00      48/250203      p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket [22]
                0.00    0.00      76/76          ksCheckCoeff(snumber **, snumber **) [135]
                0.00    0.00     152/441132      nDummy1(snumber **) [45]
                0.00    0.00      48/204         kBucketCreate(sip_sring *) [147]
                0.00    0.00     152/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
                0.00    0.00      48/392         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [240]
                                 154             yyparse(void) <cycle 1> [39]
[83]     0.0    0.00    0.01     172         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
                0.00    0.00       7/7           jjTIMES_P(sleftv *, sleftv *, sleftv *) [97]
                0.00    0.00       7/7           jjPLUS_P(sleftv *, sleftv *, sleftv *) [102]
                0.00    0.00      38/74          iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00     382/988         sleftv::CleanUp(void) [163]
                0.00    0.00     344/1176        sleftv::Typ(void) [161]
                0.00    0.00     172/239         iiTabIndex(sValCmdTab const *, int, int) [179]
                0.00    0.00      90/90          jjPLUS_S(sleftv *, sleftv *, sleftv *) [194]
                0.00    0.00      42/79          iiTestConvert(int, int) [195]
                0.00    0.00      18/18          jjGT_I(sleftv *, sleftv *, sleftv *) [239]
                0.00    0.00      14/14          jjMINUS_I(sleftv *, sleftv *, sleftv *) [251]
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [289]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [241]
-----------------------------------------------
                0.00    0.00       2/66          yy_create_buffer(_IO_FILE *, int) [124]
                0.00    0.00      64/66          myynewbuffer(void) [86]
[84]     0.0    0.00    0.00      66         yy_flex_alloc(unsigned int) [84]
                0.00    0.00      33/76          omAllocFromSystem [68]
-----------------------------------------------
                0.00    0.00      28/60          yyparse(void) <cycle 1> [39]
                0.00    0.00      32/60          iiPStart(idrec *, sleftv *) <cycle 1> [92]
[85]     0.0    0.00    0.00      60         newBuffer(char *, feBufferTypes, procinfo *, int) [85]
                0.00    0.00      32/32          myynewbuffer(void) [86]
                0.00    0.00      60/64          __builtin_new [4310]
-----------------------------------------------
                0.00    0.00      32/32          newBuffer(char *, feBufferTypes, procinfo *, int) [85]
[86]     0.0    0.00    0.00      32         myynewbuffer(void) [86]
                0.00    0.00      64/66          yy_flex_alloc(unsigned int) [84]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [204]
                0.00    0.00      32/97          yy_load_buffer_state(void) [191]
-----------------------------------------------
                0.00    0.00      26/26          omAllocBinPage [31]
[87]     0.0    0.00    0.00      26         omAllocNewBinPagesRegion [87]
                0.00    0.00      26/76          omAllocFromSystem [68]
                0.00    0.00      26/26          omRegisterBinPages [123]
                0.00    0.00      26/26          _omVallocFromSystem [4313]
-----------------------------------------------
                0.00    0.00      39/106         iiParameter(sleftv *) [103]
                0.00    0.00      67/106         yyparse(void) <cycle 1> [39]
[88]     0.0    0.00    0.00     106         iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00      93/93          jiAssign_1(sleftv *, sleftv *) [95]
                0.00    0.00       2/4           id_Delete(sip_sideal **, sip_sring *) [96]
                0.00    0.00       2/133         sleftv::CopyD(int) [93]
                0.00    0.00       2/74          iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00     214/1176        sleftv::Typ(void) [161]
                0.00    0.00     212/346         sleftv::listLength(void) [176]
                0.00    0.00     201/988         sleftv::CleanUp(void) [163]
                0.00    0.00      20/32          ipMoveId(idrec *) [226]
                0.00    0.00      12/12          jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00       3/79          iiTestConvert(int, int) [195]
                0.00    0.00       1/575         sleftv::Data(void) [166]
                0.00    0.00       1/1           exprlist_length(sleftv *) [317]
                0.00    0.00       1/6           idInit(int, int) [284]
                0.00    0.00       1/94          sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00      14/14          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[89]     0.0    0.00    0.00      14         jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
                0.00    0.00      14/14          pPower(spolyrec *, int) [91]
                0.00    0.00      14/133         sleftv::CopyD(int) [93]
                0.00    0.00      14/575         sleftv::Data(void) [166]
                0.00    0.00      14/14          jjOP_REST(sleftv *, sleftv *, sleftv *) [252]
-----------------------------------------------
                0.00    0.00       4/4           initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
[90]     0.0    0.00    0.00       4         initEcartNormal(sLObject *) [90]
                0.00    0.00       4/198         pLDeg0c(spolyrec *, int *, sip_sring *) [36]
                0.00    0.00       4/251095      pDeg(spolyrec *, sip_sring *) [38]
-----------------------------------------------
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
[91]     0.0    0.00    0.00      14         pPower(spolyrec *, int) [91]
                0.00    0.00       3/259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.00    0.00       1/1           _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00       2/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00      11/440834      npIsOne(snumber *) [41]
                0.00    0.00      11/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
                0.00    0.00       2/9           p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [261]
-----------------------------------------------
                                  32             iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
[92]     0.0    0.00    0.00      32         iiPStart(idrec *, sleftv *) <cycle 1> [92]
                0.00    0.00      32/60          newBuffer(char *, feBufferTypes, procinfo *, int) [85]
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [141]
                0.00    0.00      32/32          killlocals(int) [227]
                                  32             yyparse(void) <cycle 1> [39]
-----------------------------------------------
                0.00    0.00       1/133         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [140]
                0.00    0.00       1/133         sleftv::Print(sleftv *, int) [128]
                0.00    0.00       2/133         iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00       2/133         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [106]
                0.00    0.00      14/133         jjTIMES_P(sleftv *, sleftv *, sleftv *) [97]
                0.00    0.00      14/133         jjPLUS_P(sleftv *, sleftv *, sleftv *) [102]
                0.00    0.00      14/133         jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
                0.00    0.00      20/133         iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00      65/133         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [105]
[93]     0.0    0.00    0.00     133         sleftv::CopyD(int) [93]
                0.00    0.00      10/10          slInternalCopy(sleftv *, int, void *, _ssubexpr *) [94]
                0.00    0.00     123/560         iiCheckRing(int) [167]
                0.00    0.00      10/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00      10/10          sleftv::CopyD(int) [93]
[94]     0.0    0.00    0.00      10         slInternalCopy(sleftv *, int, void *, _ssubexpr *) [94]
                0.00    0.00       1/1           idCopy(sip_sideal *) [99]
                0.00    0.00       1/259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
-----------------------------------------------
                0.00    0.00      93/93          iiAssign(sleftv *, sleftv *) [88]
[95]     0.0    0.00    0.00      93         jiAssign_1(sleftv *, sleftv *) [95]
                0.00    0.00      65/65          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [105]
                0.00    0.00       2/2           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [106]
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [140]
                0.00    0.00     186/1176        sleftv::Typ(void) [161]
                0.00    0.00      25/25          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [233]
-----------------------------------------------
                0.00    0.00       2/4           iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00       2/4           jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [106]
[96]     0.0    0.00    0.00       4         id_Delete(sip_sideal **, sip_sring *) [96]
                0.00    0.00       8/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[97]     0.0    0.00    0.00       7         jjTIMES_P(sleftv *, sleftv *, sleftv *) [97]
                0.00    0.00       7/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00      14/133         sleftv::CopyD(int) [93]
                0.00    0.00       7/9           p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [261]
-----------------------------------------------
                0.00    0.00       7/7           enterSMora(sLObject, int, skStrategy *, int) [18]
[98]     0.0    0.00    0.00       7         newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00       7/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00       2/118         Print [73]
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
                0.00    0.00       7/251095      pDeg(spolyrec *, sip_sring *) [38]
                0.00    0.00       7/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
                0.00    0.00       6/196         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [181]
-----------------------------------------------
                0.00    0.00       1/1           slInternalCopy(sleftv *, int, void *, _ssubexpr *) [94]
[99]     0.0    0.00    0.00       1         idCopy(sip_sideal *) [99]
                0.00    0.00       3/259         p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring [35]
                0.00    0.00       1/6           idInit(int, int) [284]
-----------------------------------------------
                0.00    0.00      40/40          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[100]    0.0    0.00    0.00      40         enterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
                0.00    0.00      40/40          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [101]
                0.00    0.00      40/260805      p_GetShortExpVector(spolyrec *, sip_sring *) [27]
-----------------------------------------------
                0.00    0.00      40/40          enterpairs(spolyrec *, int, int, int, skStrategy *, int) [100]
[101]    0.0    0.00    0.00      40         initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [101]
                0.00    0.00     538/538         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
                0.00    0.00      40/40          chainCrit(spolyrec *, int, skStrategy *) [111]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[102]    0.0    0.00    0.00       7         jjPLUS_P(sleftv *, sleftv *, sleftv *) [102]
                0.00    0.00       7/39961       p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring [12]
                0.00    0.00      14/133         sleftv::CopyD(int) [93]
                0.00    0.00       7/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [190]
-----------------------------------------------
                0.00    0.00      45/45          yyparse(void) <cycle 1> [39]
[103]    0.0    0.00    0.00      45         iiParameter(sleftv *) [103]
                0.00    0.00      39/106         iiAssign(sleftv *, sleftv *) [88]
-----------------------------------------------
                0.00    0.00     538/538         initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [101]
[104]    0.0    0.00    0.00     538         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
                0.00    0.00     258/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
                0.00    0.00     258/10932       enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00     258/258         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [119]
                0.00    0.00     258/258         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.00    0.00     154/184         deleteInL(sLObject *, int *, int, skStrategy *) [146]
                0.00    0.00     538/538         pLcm(spolyrec *, spolyrec *, spolyrec *) [169]
                0.00    0.00     538/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
                0.00    0.00     538/538         pHasNotCF(spolyrec *, spolyrec *) [168]
-----------------------------------------------
                0.00    0.00      65/65          jiAssign_1(sleftv *, sleftv *) [95]
[105]    0.0    0.00    0.00      65         jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [105]
                0.00    0.00      65/133         sleftv::CopyD(int) [93]
                0.00    0.00      65/94          sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00       2/2           jiAssign_1(sleftv *, sleftv *) [95]
[106]    0.0    0.00    0.00       2         jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [106]
                0.00    0.00       2/4           id_Delete(sip_sideal **, sip_sring *) [96]
                0.00    0.00       2/133         sleftv::CopyD(int) [93]
                0.00    0.00       2/94          sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00       1/1           pPower(spolyrec *, int) [91]
[107]    0.0    0.00    0.00       1         _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00       2/240         p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring [40]
                0.00    0.00       3/23744       pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec [32]
                0.00    0.00       2/441132      nDummy1(snumber **) [45]
                0.00    0.00       1/1           npAdd(snumber *, snumber *) [340]
                0.00    0.00       1/32          npIsZero(snumber *) [229]
-----------------------------------------------
                0.00    0.00       7/7           enterSMora(sLObject, int, skStrategy *, int) [18]
[108]    0.0    0.00    0.00       7         reorderL(skStrategy *) [108]
                0.00    0.00     209/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
-----------------------------------------------
                0.00    0.00    1820/1820        yyparse(void) <cycle 1> [39]
[109]    0.0    0.00    0.00    1820         yylex(MYYSTYPE *) [109]
                0.00    0.00     789/789         yy_get_next_buffer(void) [115]
                0.00    0.00       1/1           yy_create_buffer(_IO_FILE *, int) [124]
                0.00    0.00     773/773         yy_get_previous_state(void) [164]
                0.00    0.00     512/512         IsCmd(char *, int &) [170]
                0.00    0.00      16/61          exitVoice(void) [206]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [231]
                0.00    0.00       1/97          yy_load_buffer_state(void) [191]
                0.00    0.00       1/1           m2_end [338]
-----------------------------------------------
                0.00    0.00       2/36          __gmpf_init_set_d [137]
                0.00    0.00       2/36          __gmpz_init_set_si [138]
                0.00    0.00       3/36          opendir [134]
                0.00    0.00      29/36          fopen [112]
[110]    0.0    0.00    0.00      36         malloc [110]
                0.00    0.00       3/76          omAllocFromSystem [68]
-----------------------------------------------
                0.00    0.00      40/40          initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [101]
[111]    0.0    0.00    0.00      40         chainCrit(spolyrec *, int, skStrategy *) [111]
                0.00    0.00     104/44860       posInL17(sLObject *, int, sLObject *, skStrategy *) [37]
                0.00    0.00     104/10932       enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00      27/184         deleteInL(sLObject *, int *, int, skStrategy *) [146]
                0.00    0.00     610/610         pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [165]
-----------------------------------------------
                                                 <spontaneous>
[112]    0.0    0.00    0.00                 fopen [112]
                0.00    0.00      29/36          malloc [110]
-----------------------------------------------
                0.00    0.00       2/74          iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00      11/74          iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      23/74          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      38/74          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[113]    0.0    0.00    0.00      74         iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00      20/133         sleftv::CopyD(int) [93]
                0.00    0.00      20/1176        sleftv::Typ(void) [161]
                0.00    0.00      18/18          iiS2Link(void *) [237]
                0.00    0.00       1/1           iiDummy(void *) [325]
                0.00    0.00       1/1           iiI2P(void *) [327]
-----------------------------------------------
                0.00    0.00     789/789         yy_get_next_buffer(void) [115]
[114]    0.0    0.00    0.00     789         feReadLine(char *, int) [114]
                0.00    0.00       2/76          omAllocFromSystem [68]
                0.00    0.00     378/378         fePrintEcho(char *, char *) [174]
-----------------------------------------------
                0.00    0.00     789/789         yylex(MYYSTYPE *) [109]
[115]    0.0    0.00    0.00     789         yy_get_next_buffer(void) [115]
                0.00    0.00     789/789         feReadLine(char *, int) [114]
                0.00    0.00      16/32          yyrestart(_IO_FILE *) [231]
-----------------------------------------------
                0.00    0.00       4/4           yylp_create_buffer(_IO_FILE *, int) [118]
[116]    0.0    0.00    0.00       4         yy_flex_alloc(unsigned int) [116]
                0.00    0.00       2/76          omAllocFromSystem [68]
-----------------------------------------------
                0.00    0.00       2/2           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
[117]    0.0    0.00    0.00       2         initL(void) [117]
                0.00    0.00       2/76          omAllocFromSystem [68]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[118]    0.0    0.00    0.00       2         yylp_create_buffer(_IO_FILE *, int) [118]
                0.00    0.00       4/4           yy_flex_alloc(unsigned int) [116]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [295]
-----------------------------------------------
                0.00    0.00     258/258         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[119]    0.0    0.00    0.00     258         initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [119]
                0.00    0.00     516/251095      pDeg(spolyrec *, sip_sring *) [38]
-----------------------------------------------
                0.00    0.00       1/167         global constructors keyed to ff_prime [158]
                0.00    0.00       2/167         initPT(void) [121]
                0.00    0.00      82/167         Array<CanonicalForm>::Array(int) [131]
                0.00    0.00      82/167         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [130]
[120]    0.0    0.00    0.00     167         __builtin_vec_new [120]
                0.00    0.00       1/76          omAllocFromSystem [68]
                0.00    0.00      18/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                                                 <spontaneous>
[121]    0.0    0.00    0.00                 initPT(void) [121]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(int) [131]
                0.00    0.00      82/82          Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [130]
                0.00    0.00       2/167         __builtin_vec_new [120]
                0.00    0.00     156/156         Array<CanonicalForm>::operator[](int) const [4306]
                0.00    0.00      82/82          Array<CanonicalForm>::Array(void) [4309]
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4307]
-----------------------------------------------
                0.00    0.00      26/26          omRegisterBinPages [123]
[122]    0.0    0.00    0.00      26         omBinPageIndexFault [122]
                0.00    0.00       1/76          omAllocFromSystem [68]
                0.00    0.00      25/36          omReallocSizeFromSystem [217]
-----------------------------------------------
                0.00    0.00      26/26          omAllocNewBinPagesRegion [87]
[123]    0.0    0.00    0.00      26         omRegisterBinPages [123]
                0.00    0.00      26/26          omBinPageIndexFault [122]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [109]
[124]    0.0    0.00    0.00       1         yy_create_buffer(_IO_FILE *, int) [124]
                0.00    0.00       2/66          yy_flex_alloc(unsigned int) [84]
                0.00    0.00       1/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [204]
-----------------------------------------------
                0.00    0.00       2/12          enterL(sLObject **, int *, int *, sLObject, int) [67]
                0.00    0.00      10/12          enterT(sLObject, skStrategy *, int) [33]
[125]    0.0    0.00    0.00      12         omDoRealloc [125]
                0.00    0.00       1/76          omAllocFromSystem [68]
                0.00    0.00       9/9           omRealloc0Large [260]
                0.00    0.00       2/70          omSizeOfLargeAddr [198]
                0.00    0.00       2/36          omReallocSizeFromSystem [217]
                0.00    0.00       2/2           omSizeOfAddr [308]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[126]    0.0    0.00    0.00       1         global constructors keyed to fe_promptstr [126]
                0.00    0.00       1/76          omAllocFromSystem [68]
-----------------------------------------------
                                                 <spontaneous>
[127]    0.0    0.00    0.00                 __do_global_ctors_aux [127]
                0.00    0.00       1/1           global constructors keyed to fe_promptstr [126]
                0.00    0.00       1/1           global constructors keyed to feVersionId [4326]
                0.00    0.00       1/1           global constructors keyed to dArith2 [4324]
                0.00    0.00       1/1           global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4343]
                0.00    0.00       1/1           global constructors keyed to sattr::Print(void) [4319]
                0.00    0.00       1/1           global constructors keyed to convSingNClapN(snumber *) [4323]
                0.00    0.00       1/1           global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4339]
                0.00    0.00       1/1           global constructors keyed to omSingOutOfMemoryFunc [4335]
                0.00    0.00       1/1           global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4332]
                0.00    0.00       1/1           global constructors keyed to feOptSpec [4325]
                0.00    0.00       1/1           global constructors keyed to sip_command_bin [4340]
                0.00    0.00       1/1           global constructors keyed to iiCurrArgs [4330]
                0.00    0.00       1/1           global constructors keyed to slists_bin [4341]
                0.00    0.00       1/1           global constructors keyed to rnumber_bin [4336]
                0.00    0.00       1/1           global constructors keyed to ip_smatrix_bin [4331]
                0.00    0.00       1/1           global constructors keyed to MP_INT_bin [4318]
                0.00    0.00       1/1           global constructors keyed to gmp_output_digits [4329]
                0.00    0.00       1/1           global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4342]
                0.00    0.00       1/1           global constructors keyed to fglmUpdatesource(sip_sideal *) [4327]
                0.00    0.00       1/1           global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4322]
                0.00    0.00       1/1           global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4320]
                0.00    0.00       1/1           global constructors keyed to fglmcomb.cc [4328]
                0.00    0.00       1/1           global constructors keyed to s_si_link_extension_bin [4338]
                0.00    0.00       1/1           global constructors keyed to sSubexpr_bin [4337]
                0.00    0.00       1/1           global constructors keyed to mpsr_sleftv_bin [4334]
                0.00    0.00       1/1           global constructors keyed to kBucketCreate(sip_sring *) [4333]
                0.00    0.00       1/1           global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4321]
-----------------------------------------------
                0.00    0.00      26/26          yyparse(void) <cycle 1> [39]
[128]    0.0    0.00    0.00      26         sleftv::Print(sleftv *, int) [128]
                0.00    0.00       1/118         Print [73]
                0.00    0.00       1/133         sleftv::CopyD(int) [93]
                0.00    0.00      27/1176        sleftv::Typ(void) [161]
                0.00    0.00      26/575         sleftv::Data(void) [166]
                0.00    0.00      26/988         sleftv::CleanUp(void) [163]
                0.00    0.00       1/1           PrintLn [314]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[129]    0.0    0.00    0.00       1         messageStat(int, int, int, skStrategy *) [129]
                0.00    0.00       1/118         Print [73]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [121]
[130]    0.0    0.00    0.00      82         Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [130]
                0.00    0.00      82/167         __builtin_vec_new [120]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [121]
[131]    0.0    0.00    0.00      82         Array<CanonicalForm>::Array(int) [131]
                0.00    0.00      82/167         __builtin_vec_new [120]
-----------------------------------------------
                                  88             yyparse(void) <cycle 1> [39]
[132]    0.0    0.00    0.00      88         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      88/346         sleftv::listLength(void) [176]
                0.00    0.00      88/988         sleftv::CleanUp(void) [163]
                0.00    0.00      33/33          jjSTRING_PL(sleftv *, sleftv *) [222]
                0.00    0.00      25/25          jjSYSTEM(sleftv *, sleftv *) [234]
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [337]
                                  18             jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [240]
-----------------------------------------------
                0.00    0.00      11/11          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
[133]    0.0    0.00    0.00      11         iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      11/74          iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00      22/1176        sleftv::Typ(void) [161]
                0.00    0.00      11/79          iiTestConvert(int, int) [195]
                0.00    0.00      11/575         sleftv::Data(void) [166]
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [256]
                0.00    0.00      11/988         sleftv::CleanUp(void) [163]
-----------------------------------------------
                                                 <spontaneous>
[134]    0.0    0.00    0.00                 opendir [134]
                0.00    0.00       3/36          malloc [110]
                0.00    0.00       3/3           calloc [297]
-----------------------------------------------
                0.00    0.00      76/76          ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
[135]    0.0    0.00    0.00      76         ksCheckCoeff(snumber **, snumber **) [135]
                0.00    0.00     228/440834      npIsOne(snumber *) [41]
                0.00    0.00      76/441132      nDummy1(snumber **) [45]
                0.00    0.00     152/152         ndCopy(snumber *) [188]
                0.00    0.00      76/76          ndGcd(snumber *, snumber *) [196]
-----------------------------------------------
                0.00    0.00     258/258         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[136]    0.0    0.00    0.00     258         ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.00    0.00     292/441132      nDummy1(snumber **) [45]
                0.00    0.00       3/237049      omAllocBinFromFullPage [23]
                0.00    0.00       1/236720      omFreeToPageFault [21]
                0.00    0.00     526/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
                0.00    0.00      34/50          npMult(snumber *, snumber *) [207]
                0.00    0.00      17/17          npEqual(snumber *, snumber *) [247]
-----------------------------------------------
                                                 <spontaneous>
[137]    0.0    0.00    0.00                 __gmpf_init_set_d [137]
                0.00    0.00       2/36          malloc [110]
-----------------------------------------------
                                                 <spontaneous>
[138]    0.0    0.00    0.00                 __gmpz_init_set_si [138]
                0.00    0.00       2/36          malloc [110]
-----------------------------------------------
                0.00    0.00       2/13          newFile(char *, _IO_FILE *) [145]
                0.00    0.00       2/13          iiLibCmd(char *, short) [59]
                0.00    0.00       4/13          main [1]
                0.00    0.00       5/13          iiGetLibProcBuffer(procinfo *, int) [141]
[139]    0.0    0.00    0.00      13         feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       5/236720      omFreeToPageFault [21]
                0.00    0.00       5/237049      omAllocBinFromFullPage [23]
                0.00    0.00       8/8           feResource(char, int) [263]
-----------------------------------------------
                0.00    0.00       1/1           jiAssign_1(sleftv *, sleftv *) [95]
[140]    0.0    0.00    0.00       1         jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [140]
                0.00    0.00       1/133         sleftv::CopyD(int) [93]
                0.00    0.00       1/1           pNormalize(spolyrec *) [350]
                0.00    0.00       1/94          sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00       5/5           iiPStart(idrec *, sleftv *) <cycle 1> [92]
[141]    0.0    0.00    0.00       5         iiGetLibProcBuffer(procinfo *, int) [141]
                0.00    0.00       5/13          feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       2/237049      omAllocBinFromFullPage [23]
                0.00    0.00      10/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [232]
                0.00    0.00       5/5           iiProcArgs(char *, short) [286]
-----------------------------------------------
                0.00    0.00       7/7           newHEdge(spolyrec **, int, skStrategy *) [98]
[142]    0.0    0.00    0.00       7         scComputeHC(sip_sideal *, int, spolyrec *&) [142]
                0.00    0.00       3/236720      omFreeToPageFault [21]
                0.00    0.00       7/7           hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [269]
                0.00    0.00       7/7           hCreate(int) [266]
                0.00    0.00       7/7           hStaircase(long **, int *, int *, int) [273]
                0.00    0.00       7/7           hOrdSupp(long **, int, int *, int) [272]
                0.00    0.00       7/181         hPure(long **, int, int *, int *, int, long *, int *) [183]
                0.00    0.00       7/7           hLexS(long **, int, int *, int) [271]
                0.00    0.00       7/7           hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
                0.00    0.00       7/7           hKill(monrec **, int) [270]
                0.00    0.00       7/7           hDelete(long **, int) [267]
-----------------------------------------------
                0.00    0.00      38/202         ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [8]
                0.00    0.00      40/202         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00     124/202         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
[143]    0.0    0.00    0.00     202         kBucketDestroy(kBucket **) [143]
                0.00    0.00       2/236720      omFreeToPageFault [21]
-----------------------------------------------
                                  14             jjPROC1(sleftv *, sleftv *) <cycle 1> [253]
                                  18             jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [241]
[144]    0.0    0.00    0.00      32         iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
                0.00    0.00      32/32          iiCheckNest(void) [151]
                0.00    0.00      32/33          namerec::push(sip_package *, char *, int, short) [150]
                0.00    0.00      32/32          iiConvName(char *) [225]
                0.00    0.00      32/32          namerec::pop(short) [230]
                                  32             iiPStart(idrec *, sleftv *) <cycle 1> [92]
-----------------------------------------------
                0.00    0.00       2/2           main [1]
[145]    0.0    0.00    0.00       2         newFile(char *, _IO_FILE *) [145]
                0.00    0.00       2/13          feFopen(char *, char *, char *, int, int) [139]
                0.00    0.00       2/64          __builtin_new [4310]
-----------------------------------------------
                0.00    0.00       3/184         updateLHC(skStrategy *) [26]
                0.00    0.00      27/184         chainCrit(spolyrec *, int, skStrategy *) [111]
                0.00    0.00     154/184         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[146]    0.0    0.00    0.00     184         deleteInL(sLObject *, int *, int, skStrategy *) [146]
                0.00    0.00       1/236720      omFreeToPageFault [21]
-----------------------------------------------
                0.00    0.00       4/204         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      28/204         updateLHC(skStrategy *) [26]
                0.00    0.00      48/204         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00     124/204         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
[147]    0.0    0.00    0.00     204         kBucketCreate(sip_sring *) [147]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       1/122         yyparse(void) <cycle 1> [39]
                0.00    0.00       1/122         iiLibCmd(char *, short) [59]
                0.00    0.00      41/122         yylplex(char *, char *, lib_style_types *, lp_modes) [62]
                0.00    0.00      79/122         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [157]
[148]    0.0    0.00    0.00     122         enterid(char *, int, int, idrec **, short) [148]
                0.00    0.00     122/122         idrec::set(char *, int, int, short) [149]
-----------------------------------------------
                0.00    0.00     122/122         enterid(char *, int, int, idrec **, short) [148]
[149]    0.0    0.00    0.00     122         idrec::set(char *, int, int, short) [149]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
                0.00    0.00       2/6           idInit(int, int) [284]
-----------------------------------------------
                0.00    0.00       1/33          main [1]
                0.00    0.00      32/33          iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
[150]    0.0    0.00    0.00      33         namerec::push(sip_package *, char *, int, short) [150]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
[151]    0.0    0.00    0.00      32         iiCheckNest(void) [151]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[152]    0.0    0.00    0.00       1         slStandardInit(void) [152]
                0.00    0.00       1/237049      omAllocBinFromFullPage [23]
                0.00    0.00       1/1           slInitDBMExtension(s_si_link_extension *) [371]
                0.00    0.00       1/1           slInitMPFileExtension(s_si_link_extension *) [372]
                0.00    0.00       1/1           slInitMPTcpExtension(s_si_link_extension *) [373]
-----------------------------------------------
                0.00    0.00       3/3           jjJACOB_P(sleftv *, sleftv *) [154]
[153]    0.0    0.00    0.00       3         pDiff(spolyrec *, int) [153]
                0.00    0.00      16/441132      nDummy1(snumber **) [45]
                0.00    0.00      16/267         npInit(int) [177]
                0.00    0.00      16/50          npMult(snumber *, snumber *) [207]
                0.00    0.00      16/32          npIsZero(snumber *) [229]
                0.00    0.00      16/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[154]    0.0    0.00    0.00       1         jjJACOB_P(sleftv *, sleftv *) [154]
                0.00    0.00       3/3           pDiff(spolyrec *, int) [153]
                0.00    0.00       1/6           idInit(int, int) [284]
                0.00    0.00       1/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[155]    0.0    0.00    0.00       1         idHomIdeal(sip_sideal *, sip_sideal *) [155]
                0.00    0.00       1/1           pIsHomogeneous(spolyrec *) [156]
-----------------------------------------------
                0.00    0.00       1/1           idHomIdeal(sip_sideal *, sip_sideal *) [155]
[156]    0.0    0.00    0.00       1         pIsHomogeneous(spolyrec *) [156]
                0.00    0.00       3/251095      pDeg(spolyrec *, sip_sring *) [38]
-----------------------------------------------
                0.00    0.00      79/79          yyparse(void) <cycle 1> [39]
[157]    0.0    0.00    0.00      79         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [157]
                0.00    0.00      79/122         enterid(char *, int, int, idrec **, short) [148]
                0.00    0.00      79/988         sleftv::CleanUp(void) [163]
-----------------------------------------------
                                                 <spontaneous>
[158]    0.0    0.00    0.00                 global constructors keyed to ff_prime [158]
                0.00    0.00       1/167         __builtin_vec_new [120]
-----------------------------------------------
[159]    0.0    0.00    0.00      12+18      <cycle 2 as a whole> [159]
                0.00    0.00      20             feResource(feResourceConfig_s *, int) <cycle 2> [236]
                0.00    0.00       6             feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
-----------------------------------------------
                0.00    0.00       7/1477        newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00      11/1477        pPower(spolyrec *, int) [91]
                0.00    0.00      14/1477        syMake(sleftv *, char *, idrec *) [49]
                0.00    0.00      16/1477        pDiff(spolyrec *, int) [153]
                0.00    0.00     152/1477        ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00     213/1477        hHedge(spolyrec *) [182]
                0.00    0.00     526/1477        ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
                0.00    0.00     538/1477        enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[160]    0.0    0.00    0.00    1477         p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
-----------------------------------------------
                                   4             sleftv::Typ(void) [161]
                0.00    0.00       1/1176        setOption(sleftv *, sleftv *) [370]
                0.00    0.00       1/1176        rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.00       2/1176        exprlist_length(sleftv *) [317]
                0.00    0.00      12/1176        jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      20/1176        iiConvert(int, int, int, sleftv *, sleftv *) [113]
                0.00    0.00      21/1176        iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [274]
                0.00    0.00      22/1176        iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      27/1176        sleftv::Print(sleftv *, int) [128]
                0.00    0.00      32/1176        sleftv::Copy(sleftv *) [223]
                0.00    0.00      43/1176        jjSYSTEM(sleftv *, sleftv *) [234]
                0.00    0.00      44/1176        sleftv::String(void *, short, int) [209]
                0.00    0.00      64/1176        killlocals(int) [227]
                0.00    0.00      67/1176        iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      76/1176        yyparse(void) <cycle 1> [39]
                0.00    0.00     186/1176        jiAssign_1(sleftv *, sleftv *) [95]
                0.00    0.00     214/1176        iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00     344/1176        iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[161]    0.0    0.00    0.00    1176+4       sleftv::Typ(void) [161]
                                   4             sleftv::Typ(void) [161]
-----------------------------------------------
                0.00    0.00    1124/1124        omFreeToPageFault [21]
[162]    0.0    0.00    0.00    1124         omFreeBinPages [162]
-----------------------------------------------
                                  48             sleftv::CleanUp(void) [163]
                0.00    0.00       1/988         iiExport(sleftv *, int) [326]
                0.00    0.00       2/988         rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.00      11/988         iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      12/988         jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      12/988         killhdl(idrec *, idrec **) [200]
                0.00    0.00      21/988         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [274]
                0.00    0.00      26/988         sleftv::Print(sleftv *, int) [128]
                0.00    0.00      63/988         yyparse(void) <cycle 1> [39]
                0.00    0.00      79/988         iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [157]
                0.00    0.00      88/988         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
                0.00    0.00      90/988         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     201/988         iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00     382/988         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[163]    0.0    0.00    0.00     988+48      sleftv::CleanUp(void) [163]
                0.00    0.00      18/18          slKill(sip_link *) [245]
                0.00    0.00       1/63          __builtin_delete [4311]
                                  48             sleftv::CleanUp(void) [163]
-----------------------------------------------
                0.00    0.00     773/773         yylex(MYYSTYPE *) [109]
[164]    0.0    0.00    0.00     773         yy_get_previous_state(void) [164]
-----------------------------------------------
                0.00    0.00     610/610         chainCrit(spolyrec *, int, skStrategy *) [111]
[165]    0.0    0.00    0.00     610         pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *) [165]
-----------------------------------------------
                                   4             sleftv::Data(void) [166]
                0.00    0.00       1/575         jjSTD(sleftv *, sleftv *) [5]
                0.00    0.00       1/575         jjJACOB_P(sleftv *, sleftv *) [154]
                0.00    0.00       1/575         iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00       1/575         exprlist_length(sleftv *) [317]
                0.00    0.00       1/575         rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.00       3/575         jjMEMORY(sleftv *, sleftv *) [74]
                0.00    0.00       4/575         jjMakeSub(sleftv *) [290]
                0.00    0.00       7/575         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00       7/575         jjREAD(sleftv *, sleftv *) [276]
                0.00    0.00      10/575         sleftv::CopyD(int) [93]
                0.00    0.00      11/575         iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      12/575         jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      14/575         jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
                0.00    0.00      18/575         jjCOUNT_L(sleftv *, sleftv *) [238]
                0.00    0.00      21/575         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [275]
                0.00    0.00      25/575         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [233]
                0.00    0.00      26/575         sleftv::Print(sleftv *, int) [128]
                0.00    0.00      28/575         jjMINUS_I(sleftv *, sleftv *, sleftv *) [251]
                0.00    0.00      32/575         sleftv::Copy(sleftv *) [223]
                0.00    0.00      36/575         jjGT_I(sleftv *, sleftv *, sleftv *) [239]
                0.00    0.00      41/575         jjSYSTEM(sleftv *, sleftv *) [234]
                0.00    0.00      44/575         sleftv::String(void *, short, int) [209]
                0.00    0.00      51/575         yyparse(void) <cycle 1> [39]
                0.00    0.00     180/575         jjPLUS_S(sleftv *, sleftv *, sleftv *) [194]
[166]    0.0    0.00    0.00     575+4       sleftv::Data(void) [166]
                0.00    0.00     437/560         iiCheckRing(int) [167]
                0.00    0.00       2/2           getTimer(void) [305]
                                   4             sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00     123/560         sleftv::CopyD(int) [93]
                0.00    0.00     437/560         sleftv::Data(void) [166]
[167]    0.0    0.00    0.00     560         iiCheckRing(int) [167]
-----------------------------------------------
                0.00    0.00     538/538         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[168]    0.0    0.00    0.00     538         pHasNotCF(spolyrec *, spolyrec *) [168]
-----------------------------------------------
                0.00    0.00     538/538         enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [104]
[169]    0.0    0.00    0.00     538         pLcm(spolyrec *, spolyrec *, spolyrec *) [169]
-----------------------------------------------
                0.00    0.00     512/512         yylex(MYYSTYPE *) [109]
[170]    0.0    0.00    0.00     512         IsCmd(char *, int &) [170]
-----------------------------------------------
                0.00    0.00     459/459         cancelunit(sLObject *) [180]
[171]    0.0    0.00    0.00     459         rIsPolyVar(int, sip_sring *) [171]
-----------------------------------------------
                0.00    0.00     403/403         yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[172]    0.0    0.00    0.00     403         current_pos(int) [172]
-----------------------------------------------
                0.00    0.00       4/392         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      28/392         updateLHC(skStrategy *) [26]
                0.00    0.00      48/392         ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [82]
                0.00    0.00     124/392         doRed(sLObject *, sTObject *, short, skStrategy *) [14]
                0.00    0.00     188/392         deleteHC(sLObject *, skStrategy *, short) [17]
[173]    0.0    0.00    0.00     392         kBucketInit(kBucket *, spolyrec *, int) [173]
-----------------------------------------------
                0.00    0.00     378/378         feReadLine(char *, int) [114]
[174]    0.0    0.00    0.00     378         fePrintEcho(char *, char *) [174]
-----------------------------------------------
                0.00    0.00     347/347         yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[175]    0.0    0.00    0.00     347         copy_string(lp_modes) [175]
-----------------------------------------------
                0.00    0.00       1/346         rInit(sleftv *, sleftv *, sleftv *) [57]
                0.00    0.00      12/346         jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      33/346         jjSTRING_PL(sleftv *, sleftv *) [222]
                0.00    0.00      88/346         iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
                0.00    0.00     212/346         iiAssign(sleftv *, sleftv *) [88]
[176]    0.0    0.00    0.00     346         sleftv::listLength(void) [176]
-----------------------------------------------
                0.00    0.00       1/267         nInitChar(sip_sring *) [63]
                0.00    0.00      15/267         p_ISet(int, sip_sring *) [249]
                0.00    0.00      16/267         pDiff(spolyrec *, int) [153]
                0.00    0.00      76/267         ndGcd(snumber *, snumber *) [196]
                0.00    0.00     159/267         pNorm(spolyrec *) [25]
[177]    0.0    0.00    0.00     267         npInit(int) [177]
-----------------------------------------------
                0.00    0.00     242/242         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[178]    0.0    0.00    0.00     242         hStepS(long **, int, int *, int, int *, long *) [178]
-----------------------------------------------
                0.00    0.00      67/239         iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00     172/239         iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[179]    0.0    0.00    0.00     239         iiTabIndex(sValCmdTab const *, int, int) [179]
-----------------------------------------------
                0.00    0.00       4/202         initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00      40/202         mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00     158/202         updateT(skStrategy *) [29]
[180]    0.0    0.00    0.00     202         cancelunit(sLObject *) [180]
                0.00    0.00     459/459         rIsPolyVar(int, sip_sring *) [171]
-----------------------------------------------
                0.00    0.00       6/196         newHEdge(spolyrec **, int, skStrategy *) [98]
                0.00    0.00     190/196         deleteHC(sLObject *, skStrategy *, short) [17]
[181]    0.0    0.00    0.00     196         p_LmCmp(spolyrec *, spolyrec *, sip_sring *) [181]
-----------------------------------------------
                0.00    0.00     181/181         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[182]    0.0    0.00    0.00     181         hHedge(spolyrec *) [182]
                0.00    0.00     213/1477        p_Setm_TotalDegree(spolyrec *, sip_sring *) [160]
-----------------------------------------------
                0.00    0.00       7/181         scComputeHC(sip_sideal *, int, spolyrec *&) [142]
                0.00    0.00     174/181         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[183]    0.0    0.00    0.00     181         hPure(long **, int, int *, int *, int, long *, int *) [183]
-----------------------------------------------
                0.00    0.00     174/174         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[184]    0.0    0.00    0.00     174         hElimS(long **, int *, int, int, int *, int) [184]
-----------------------------------------------
                0.00    0.00     174/174         hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[185]    0.0    0.00    0.00     174         hLex2S(long **, int, int, int, int *, int, long **) [185]
-----------------------------------------------
                0.00    0.00     170/170         idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [264]
[186]    0.0    0.00    0.00     170         p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [186]
-----------------------------------------------
                0.00    0.00     158/158         enterT(sLObject, skStrategy *, int) [33]
[187]    0.0    0.00    0.00     158         posInT17(sTObject *, int, sLObject const &) [187]
-----------------------------------------------
                0.00    0.00     152/152         ksCheckCoeff(snumber **, snumber **) [135]
[188]    0.0    0.00    0.00     152         ndCopy(snumber *) [188]
-----------------------------------------------
                0.00    0.00      23/121         syMake(sleftv *, char *, idrec *) [49]
                0.00    0.00      98/121         pmInit(char *, short &) [58]
[189]    0.0    0.00    0.00     121         rIsRingVar(char *) [189]
-----------------------------------------------
                0.00    0.00       7/111         jjPLUS_P(sleftv *, sleftv *, sleftv *) [102]
                0.00    0.00      14/111         jjMINUS_I(sleftv *, sleftv *, sleftv *) [251]
                0.00    0.00      90/111         jjPLUS_S(sleftv *, sleftv *, sleftv *) [194]
[190]    0.0    0.00    0.00     111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [190]
-----------------------------------------------
                0.00    0.00       1/97          yylex(MYYSTYPE *) [109]
                0.00    0.00      32/97          yyrestart(_IO_FILE *) [231]
                0.00    0.00      32/97          myynewbuffer(void) [86]
                0.00    0.00      32/97          myyoldbuffer(void *) [228]
[191]    0.0    0.00    0.00      97         yy_load_buffer_state(void) [191]
-----------------------------------------------
                0.00    0.00       1/94          jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [140]
                0.00    0.00       1/94          iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00       2/94          jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [106]
                0.00    0.00      25/94          jiA_INT(sleftv *, sleftv *, _ssubexpr *) [233]
                0.00    0.00      65/94          jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [105]
[192]    0.0    0.00    0.00      94         sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00      17/93          rEALLOc [218]
                0.00    0.00      76/93          omAllocFromSystem [68]
[193]    0.0    0.00    0.00      93         mALLOc [193]
                0.00    0.00      10/10          malloc_extend_top [257]
-----------------------------------------------
                0.00    0.00      90/90          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[194]    0.0    0.00    0.00      90         jjPLUS_S(sleftv *, sleftv *, sleftv *) [194]
                0.00    0.00     180/575         sleftv::Data(void) [166]
                0.00    0.00      90/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [190]
-----------------------------------------------
                0.00    0.00       3/79          iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00      11/79          iiWRITE(sleftv *, sleftv *) [133]
                0.00    0.00      23/79          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
                0.00    0.00      42/79          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[195]    0.0    0.00    0.00      79         iiTestConvert(int, int) [195]
-----------------------------------------------
                0.00    0.00      76/76          ksCheckCoeff(snumber **, snumber **) [135]
[196]    0.0    0.00    0.00      76         ndGcd(snumber *, snumber *) [196]
                0.00    0.00      76/267         npInit(int) [177]
-----------------------------------------------
                0.00    0.00       1/71          omSizeOfAddr [308]
                0.00    0.00      70/71          omSizeOfLargeAddr [198]
[197]    0.0    0.00    0.00      71         malloc_usable_size [197]
-----------------------------------------------
                0.00    0.00       1/70          exitVoice(void) [206]
                0.00    0.00       2/70          yy_flex_free(void *) [293]
                0.00    0.00       2/70          omDoRealloc [125]
                0.00    0.00       3/70          exitBuchMora(skStrategy *) [43]
                0.00    0.00       3/70          free [219]
                0.00    0.00      27/70          omRealloc0Large [260]
                0.00    0.00      32/70          yy_flex_free(void *) [205]
[198]    0.0    0.00    0.00      70         omSizeOfLargeAddr [198]
                0.00    0.00      70/71          malloc_usable_size [197]
-----------------------------------------------
                0.00    0.00      28/69          rEALLOc [218]
                0.00    0.00      41/69          omFreeSizeToSystem [211]
[199]    0.0    0.00    0.00      69         fREe [199]
-----------------------------------------------
                0.00    0.00      69/69          killlocals(int) [227]
[200]    0.0    0.00    0.00      69         killhdl(idrec *, idrec **) [200]
                0.00    0.00      12/988         sleftv::CleanUp(void) [163]
-----------------------------------------------
                0.00    0.00      68/68          hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[201]    0.0    0.00    0.00      68         hGetmem(int, long **, monrec *) [201]
-----------------------------------------------
                0.00    0.00      68/68          hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
[202]    0.0    0.00    0.00      68         hGetpure(long *) [202]
-----------------------------------------------
                0.00    0.00      65/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [204]
[203]    0.0    0.00    0.00      65         yy_flush_buffer(yy_buffer_state *) [203]
-----------------------------------------------
                0.00    0.00       1/65          yy_create_buffer(_IO_FILE *, int) [124]
                0.00    0.00      32/65          yyrestart(_IO_FILE *) [231]
                0.00    0.00      32/65          myynewbuffer(void) [86]
[204]    0.0    0.00    0.00      65         yy_init_buffer(yy_buffer_state *, _IO_FILE *) [204]
                0.00    0.00      65/65          yy_flush_buffer(yy_buffer_state *) [203]
-----------------------------------------------
                0.00    0.00      64/64          myyoldbuffer(void *) [228]
[205]    0.0    0.00    0.00      64         yy_flex_free(void *) [205]
                0.00    0.00      32/70          omSizeOfLargeAddr [198]
                0.00    0.00      32/41          omFreeSizeToSystem [211]
-----------------------------------------------
                0.00    0.00      16/61          yylex(MYYSTYPE *) [109]
                0.00    0.00      45/61          exitBuffer(feBufferTypes) [224]
[206]    0.0    0.00    0.00      61         exitVoice(void) [206]
                0.00    0.00      61/63          __builtin_delete [4311]
                0.00    0.00      32/32          myyoldbuffer(void *) [228]
                0.00    0.00       1/70          omSizeOfLargeAddr [198]
                0.00    0.00       1/41          omFreeSizeToSystem [211]
-----------------------------------------------
                0.00    0.00      16/50          pDiff(spolyrec *, int) [153]
                0.00    0.00      34/50          ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
[207]    0.0    0.00    0.00      50         npMult(snumber *, snumber *) [207]
-----------------------------------------------
                0.00    0.00      49/49          pmInit(char *, short &) [58]
[208]    0.0    0.00    0.00      49         npRead(char *, snumber **) [208]
-----------------------------------------------
                0.00    0.00      11/44          slWriteAscii(sip_link *, sleftv *) [255]
                0.00    0.00      33/44          jjSTRING_PL(sleftv *, sleftv *) [222]
[209]    0.0    0.00    0.00      44         sleftv::String(void *, short, int) [209]
                0.00    0.00      44/575         sleftv::Data(void) [166]
                0.00    0.00      44/1176        sleftv::Typ(void) [161]
-----------------------------------------------
                0.00    0.00      41/41          yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[210]    0.0    0.00    0.00      41         iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [210]
-----------------------------------------------
                0.00    0.00       1/41          exitVoice(void) [206]
                0.00    0.00       2/41          yy_flex_free(void *) [293]
                0.00    0.00       3/41          exitBuchMora(skStrategy *) [43]
                0.00    0.00       3/41          free [219]
                0.00    0.00      32/41          yy_flex_free(void *) [205]
[211]    0.0    0.00    0.00      41         omFreeSizeToSystem [211]
                0.00    0.00      41/69          fREe [199]
-----------------------------------------------
                0.00    0.00      40/40          enterSMora(sLObject, int, skStrategy *, int) [18]
[212]    0.0    0.00    0.00      40         HEckeTest(spolyrec *, skStrategy *) [212]
                0.00    0.00      40/40          p_IsPurePower(spolyrec *, sip_sring *) [214]
-----------------------------------------------
                0.00    0.00      40/40          enterSMora(sLObject, int, skStrategy *, int) [18]
[213]    0.0    0.00    0.00      40         enterSBba(sLObject, int, skStrategy *, int) [213]
                0.00    0.00       1/2           pEnlargeSet(spolyrec ***, int, int) [309]
-----------------------------------------------
                0.00    0.00      40/40          HEckeTest(spolyrec *, skStrategy *) [212]
[214]    0.0    0.00    0.00      40         p_IsPurePower(spolyrec *, sip_sring *) [214]
-----------------------------------------------
                0.00    0.00      40/40          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[215]    0.0    0.00    0.00      40         redtail(spolyrec *, int, skStrategy *) [215]
-----------------------------------------------
                0.00    0.00      39/39          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[216]    0.0    0.00    0.00      39         posInS(spolyrec **, int, spolyrec *) [216]
-----------------------------------------------
                0.00    0.00       2/36          omDoRealloc [125]
                0.00    0.00       9/36          omRealloc0Large [260]
                0.00    0.00      25/36          omBinPageIndexFault [122]
[217]    0.0    0.00    0.00      36         omReallocSizeFromSystem [217]
                0.00    0.00      36/36          rEALLOc [218]
-----------------------------------------------
                0.00    0.00      36/36          omReallocSizeFromSystem [217]
[218]    0.0    0.00    0.00      36         rEALLOc [218]
                0.00    0.00      28/69          fREe [199]
                0.00    0.00      17/93          mALLOc [193]
-----------------------------------------------
                0.00    0.00       6/34          closedir [1007]
                0.00    0.00      28/34          fclose [1242]
[219]    0.0    0.00    0.00      34         free [219]
                0.00    0.00       3/70          omSizeOfLargeAddr [198]
                0.00    0.00       3/41          omFreeSizeToSystem [211]
-----------------------------------------------
                0.00    0.00       1/33          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       4/33          mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
                0.00    0.00      28/33          updateLHC(skStrategy *) [26]
[220]    0.0    0.00    0.00      33         sLObject::GetpLength(void) [220]
-----------------------------------------------
                0.00    0.00       4/33          feCleanResourceValue(feResourceType, char *) [281]
                0.00    0.00      29/33          feCleanUpPath(char *) [300]
[221]    0.0    0.00    0.00      33         feCleanUpFile(char *) [221]
                0.00    0.00       4/4           mystrcpy(char *, char *) [292]
-----------------------------------------------
                0.00    0.00      33/33          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
[222]    0.0    0.00    0.00      33         jjSTRING_PL(sleftv *, sleftv *) [222]
                0.00    0.00      33/346         sleftv::listLength(void) [176]
                0.00    0.00      33/44          sleftv::String(void *, short, int) [209]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      20/32          yyparse(void) <cycle 1> [39]
[223]    0.0    0.00    0.00      32         sleftv::Copy(sleftv *) [223]
                0.00    0.00      32/1176        sleftv::Typ(void) [161]
                0.00    0.00      32/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00      32/32          yyparse(void) <cycle 1> [39]
[224]    0.0    0.00    0.00      32         exitBuffer(feBufferTypes) [224]
                0.00    0.00      45/61          exitVoice(void) [206]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
[225]    0.0    0.00    0.00      32         iiConvName(char *) [225]
-----------------------------------------------
                0.00    0.00      12/32          jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      20/32          iiAssign(sleftv *, sleftv *) [88]
[226]    0.0    0.00    0.00      32         ipMoveId(idrec *) [226]
                0.00    0.00       9/9           lRingDependend(slists *) [258]
-----------------------------------------------
                0.00    0.00      32/32          iiPStart(idrec *, sleftv *) <cycle 1> [92]
[227]    0.0    0.00    0.00      32         killlocals(int) [227]
                0.00    0.00      69/69          killhdl(idrec *, idrec **) [200]
                0.00    0.00      64/1176        sleftv::Typ(void) [161]
-----------------------------------------------
                0.00    0.00      32/32          exitVoice(void) [206]
[228]    0.0    0.00    0.00      32         myyoldbuffer(void *) [228]
                0.00    0.00      64/64          yy_flex_free(void *) [205]
                0.00    0.00      32/97          yy_load_buffer_state(void) [191]
-----------------------------------------------
                0.00    0.00       1/32          _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
                0.00    0.00      15/32          p_ISet(int, sip_sring *) [249]
                0.00    0.00      16/32          pDiff(spolyrec *, int) [153]
[229]    0.0    0.00    0.00      32         npIsZero(snumber *) [229]
-----------------------------------------------
                0.00    0.00      32/32          iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
[230]    0.0    0.00    0.00      32         namerec::pop(short) [230]
-----------------------------------------------
                0.00    0.00      16/32          yylex(MYYSTYPE *) [109]
                0.00    0.00      16/32          yy_get_next_buffer(void) [115]
[231]    0.0    0.00    0.00      32         yyrestart(_IO_FILE *) [231]
                0.00    0.00      32/65          yy_init_buffer(yy_buffer_state *, _IO_FILE *) [204]
                0.00    0.00      32/97          yy_load_buffer_state(void) [191]
-----------------------------------------------
                0.00    0.00       7/26          slReadAscii2(sip_link *, sleftv *) [277]
                0.00    0.00       9/26          libread(_IO_FILE *, char *, int) [259]
                0.00    0.00      10/26          iiGetLibProcBuffer(procinfo *, int) [141]
[232]    0.0    0.00    0.00      26         myfread(void *, unsigned int, unsigned int, _IO_FILE *) [232]
-----------------------------------------------
                0.00    0.00      25/25          jiAssign_1(sleftv *, sleftv *) [95]
[233]    0.0    0.00    0.00      25         jiA_INT(sleftv *, sleftv *, _ssubexpr *) [233]
                0.00    0.00      25/575         sleftv::Data(void) [166]
                0.00    0.00      25/94          sleftv::LData(void) [192]
-----------------------------------------------
                0.00    0.00      25/25          iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
[234]    0.0    0.00    0.00      25         jjSYSTEM(sleftv *, sleftv *) [234]
                0.00    0.00      43/1176        sleftv::Typ(void) [161]
                0.00    0.00      41/575         sleftv::Data(void) [166]
                0.00    0.00       2/2           feGetOptIndex(char const *) [301]
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [321]
-----------------------------------------------
                0.00    0.00       1/20          feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
                0.00    0.00       4/20          feInitResources(char *) [319]
                0.00    0.00       7/20          feSprintf(char *, char const *, int) <cycle 2> [287]
                0.00    0.00       8/20          feResource(char, int) [263]
[235]    0.0    0.00    0.00      20         feGetResourceConfig(char) [235]
-----------------------------------------------
                                   1             feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
                                   7             feSprintf(char *, char const *, int) <cycle 2> [287]
                0.00    0.00       4/12          feInitResources(char *) [319]
                0.00    0.00       8/12          feResource(char, int) [263]
[236]    0.0    0.00    0.00      20         feResource(feResourceConfig_s *, int) <cycle 2> [236]
                                   6             feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
-----------------------------------------------
                0.00    0.00      18/18          iiConvert(int, int, int, sleftv *, sleftv *) [113]
[237]    0.0    0.00    0.00      18         iiS2Link(void *) [237]
                0.00    0.00      18/18          slInit(sip_link *, char *) [244]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[238]    0.0    0.00    0.00      18         jjCOUNT_L(sleftv *, sleftv *) [238]
                0.00    0.00      18/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00      18/18          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[239]    0.0    0.00    0.00      18         jjGT_I(sleftv *, sleftv *, sleftv *) [239]
                0.00    0.00      36/575         sleftv::Data(void) [166]
-----------------------------------------------
                                  18             iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
[240]    0.0    0.00    0.00      18         jjKLAMMER_PL(sleftv *, sleftv *) <cycle 1> [240]
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
-----------------------------------------------
                                  18             iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[241]    0.0    0.00    0.00      18         jjPROC(sleftv *, sleftv *, sleftv *) <cycle 1> [241]
                                  18             iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
-----------------------------------------------
                0.00    0.00      18/18          slKill(sip_link *) [245]
[242]    0.0    0.00    0.00      18         slCleanUp(sip_link *) [242]
                0.00    0.00      18/18          slCloseAscii(sip_link *) [243]
-----------------------------------------------
                0.00    0.00      18/18          slCleanUp(sip_link *) [242]
[243]    0.0    0.00    0.00      18         slCloseAscii(sip_link *) [243]
-----------------------------------------------
                0.00    0.00      18/18          iiS2Link(void *) [237]
[244]    0.0    0.00    0.00      18         slInit(sip_link *, char *) [244]
-----------------------------------------------
                0.00    0.00      18/18          sleftv::CleanUp(void) [163]
[245]    0.0    0.00    0.00      18         slKill(sip_link *) [245]
                0.00    0.00      18/18          slCleanUp(sip_link *) [242]
-----------------------------------------------
                0.00    0.00       7/18          slRead(sip_link *, sleftv *) [279]
                0.00    0.00      11/18          slWrite(sip_link *, sleftv *) [256]
[246]    0.0    0.00    0.00      18         slOpenAscii(sip_link *, short) [246]
-----------------------------------------------
                0.00    0.00      17/17          ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [136]
[247]    0.0    0.00    0.00      17         npEqual(snumber *, snumber *) [247]
-----------------------------------------------
                0.00    0.00      16/16          iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[248]    0.0    0.00    0.00      16         jjDEFINED(sleftv *, sleftv *) [248]
-----------------------------------------------
                0.00    0.00       1/15          iiI2P(void *) [327]
                0.00    0.00      14/15          syMake(sleftv *, char *, idrec *) [49]
[249]    0.0    0.00    0.00      15         p_ISet(int, sip_sring *) [249]
                0.00    0.00      15/267         npInit(int) [177]
                0.00    0.00      15/32          npIsZero(snumber *) [229]
-----------------------------------------------
                0.00    0.00      14/14          SetProcs(p_Field, p_Length, p_Ord) [315]
[250]    0.0    0.00    0.00      14         FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [250]
                0.00    0.00       2/2           ZeroOrd_2_NonZeroOrd(p_Ord, int) [299]
-----------------------------------------------
                0.00    0.00      14/14          iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[251]    0.0    0.00    0.00      14         jjMINUS_I(sleftv *, sleftv *, sleftv *) [251]
                0.00    0.00      28/575         sleftv::Data(void) [166]
                0.00    0.00      14/111         jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [190]
-----------------------------------------------
                0.00    0.00      14/14          jjPOWER_P(sleftv *, sleftv *, sleftv *) [89]
[252]    0.0    0.00    0.00      14         jjOP_REST(sleftv *, sleftv *, sleftv *) [252]
-----------------------------------------------
                                  14             iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[253]    0.0    0.00    0.00      14         jjPROC1(sleftv *, sleftv *) <cycle 1> [253]
                                  14             iiMake_proc(idrec *, sleftv *) <cycle 1> [144]
-----------------------------------------------
                0.00    0.00      12/12          iiAssign(sleftv *, sleftv *) [88]
[254]    0.0    0.00    0.00      12         jjA_L_LIST(sleftv *, sleftv *) [254]
                0.00    0.00      12/346         sleftv::listLength(void) [176]
                0.00    0.00      12/1176        sleftv::Typ(void) [161]
                0.00    0.00      12/32          sleftv::Copy(sleftv *) [223]
                0.00    0.00      12/575         sleftv::Data(void) [166]
                0.00    0.00      12/32          ipMoveId(idrec *) [226]
                0.00    0.00      12/988         sleftv::CleanUp(void) [163]
-----------------------------------------------
                0.00    0.00      11/11          slWrite(sip_link *, sleftv *) [256]
[255]    0.0    0.00    0.00      11         slWriteAscii(sip_link *, sleftv *) [255]
                0.00    0.00      11/44          sleftv::String(void *, short, int) [209]
-----------------------------------------------
                0.00    0.00      11/11          iiWRITE(sleftv *, sleftv *) [133]
[256]    0.0    0.00    0.00      11         slWrite(sip_link *, sleftv *) [256]
                0.00    0.00      11/18          slOpenAscii(sip_link *, short) [246]
                0.00    0.00      11/11          slWriteAscii(sip_link *, sleftv *) [255]
-----------------------------------------------
                0.00    0.00      10/10          mALLOc [193]
[257]    0.0    0.00    0.00      10         malloc_extend_top [257]
-----------------------------------------------
                0.00    0.00       9/9           ipMoveId(idrec *) [226]
[258]    0.0    0.00    0.00       9         lRingDependend(slists *) [258]
-----------------------------------------------
                0.00    0.00       9/9           yy_get_next_buffer(void) [262]
[259]    0.0    0.00    0.00       9         libread(_IO_FILE *, char *, int) [259]
                0.00    0.00       9/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [232]
-----------------------------------------------
                0.00    0.00       9/9           omDoRealloc [125]
[260]    0.0    0.00    0.00       9         omRealloc0Large [260]
                0.00    0.00      27/70          omSizeOfLargeAddr [198]
                0.00    0.00       9/36          omReallocSizeFromSystem [217]
-----------------------------------------------
                0.00    0.00       2/9           pPower(spolyrec *, int) [91]
                0.00    0.00       7/9           jjTIMES_P(sleftv *, sleftv *, sleftv *) [97]
[261]    0.0    0.00    0.00       9         p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring [261]
-----------------------------------------------
                0.00    0.00       9/9           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[262]    0.0    0.00    0.00       9         yy_get_next_buffer(void) [262]
                0.00    0.00       9/9           libread(_IO_FILE *, char *, int) [259]
                0.00    0.00       2/2           yylprestart(_IO_FILE *) [311]
-----------------------------------------------
                0.00    0.00       8/8           feFopen(char *, char *, char *, int, int) [139]
[263]    0.0    0.00    0.00       8         feResource(char, int) [263]
                0.00    0.00       8/20          feGetResourceConfig(char) [235]
                0.00    0.00       8/12          feResource(feResourceConfig_s *, int) <cycle 2> [236]
-----------------------------------------------
                0.00    0.00       1/8           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       7/8           hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [269]
[264]    0.0    0.00    0.00       8         idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [264]
                0.00    0.00     170/170         p_MaxComp(spolyrec *, sip_sring *, sip_sring *) [186]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [279]
[265]    0.0    0.00    0.00       7         sleftv::Eval(void) [265]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[266]    0.0    0.00    0.00       7         hCreate(int) [266]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[267]    0.0    0.00    0.00       7         hDelete(long **, int) [267]
-----------------------------------------------
                                 242             hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[268]    0.0    0.00    0.00       7+242     hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
                0.00    0.00     242/242         hStepS(long **, int, int *, int, int *, long *) [178]
                0.00    0.00     181/181         hHedge(spolyrec *) [182]
                0.00    0.00     174/174         hElimS(long **, int *, int, int, int *, int) [184]
                0.00    0.00     174/181         hPure(long **, int, int *, int *, int, long *, int *) [183]
                0.00    0.00     174/174         hLex2S(long **, int, int, int, int *, int, long **) [185]
                0.00    0.00      68/68          hGetpure(long *) [202]
                0.00    0.00      68/68          hGetmem(int, long **, monrec *) [201]
                                 242             hHedgeStep(long *, long **, int, int *, int, spolyrec *) [268]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[269]    0.0    0.00    0.00       7         hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [269]
                0.00    0.00       7/8           idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [264]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[270]    0.0    0.00    0.00       7         hKill(monrec **, int) [270]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[271]    0.0    0.00    0.00       7         hLexS(long **, int, int *, int) [271]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[272]    0.0    0.00    0.00       7         hOrdSupp(long **, int, int *, int) [272]
-----------------------------------------------
                0.00    0.00       7/7           scComputeHC(sip_sideal *, int, spolyrec *&) [142]
[273]    0.0    0.00    0.00       7         hStaircase(long **, int *, int *, int) [273]
-----------------------------------------------
                0.00    0.00       7/7           yyparse(void) <cycle 1> [39]
[274]    0.0    0.00    0.00       7         iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [274]
                0.00    0.00      21/1176        sleftv::Typ(void) [161]
                0.00    0.00      21/988         sleftv::CleanUp(void) [163]
                0.00    0.00       7/7           jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [275]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [274]
[275]    0.0    0.00    0.00       7         jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [275]
                0.00    0.00      21/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00       7/7           iiExprArith1(sleftv *, sleftv *, int) <cycle 1> [4]
[276]    0.0    0.00    0.00       7         jjREAD(sleftv *, sleftv *) [276]
                0.00    0.00       7/575         sleftv::Data(void) [166]
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [279]
-----------------------------------------------
                0.00    0.00       7/7           slReadAscii(sip_link *) [278]
[277]    0.0    0.00    0.00       7         slReadAscii2(sip_link *, sleftv *) [277]
                0.00    0.00       7/26          myfread(void *, unsigned int, unsigned int, _IO_FILE *) [232]
-----------------------------------------------
                0.00    0.00       7/7           slRead(sip_link *, sleftv *) [279]
[278]    0.0    0.00    0.00       7         slReadAscii(sip_link *) [278]
                0.00    0.00       7/7           slReadAscii2(sip_link *, sleftv *) [277]
-----------------------------------------------
                0.00    0.00       7/7           jjREAD(sleftv *, sleftv *) [276]
[279]    0.0    0.00    0.00       7         slRead(sip_link *, sleftv *) [279]
                0.00    0.00       7/18          slOpenAscii(sip_link *, short) [246]
                0.00    0.00       7/7           slReadAscii(sip_link *) [278]
                0.00    0.00       7/7           sleftv::Eval(void) [265]
-----------------------------------------------
                0.00    0.00       7/7           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[280]    0.0    0.00    0.00       7         yy_get_previous_state(void) [280]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
[281]    0.0    0.00    0.00       6         feCleanResourceValue(feResourceType, char *) [281]
                0.00    0.00       4/33          feCleanUpFile(char *) [221]
                0.00    0.00       2/2           feCleanUpPath(char *) [300]
-----------------------------------------------
                                   6             feResource(feResourceConfig_s *, int) <cycle 2> [236]
[282]    0.0    0.00    0.00       6         feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
                0.00    0.00       6/6           feCleanResourceValue(feResourceType, char *) [281]
                0.00    0.00       6/6           feVerifyResourceValue(feResourceType, char *) [283]
                0.00    0.00       1/1           feGetExpandedExecutable(void) [318]
                0.00    0.00       1/20          feGetResourceConfig(char) [235]
                                   4             feSprintf(char *, char const *, int) <cycle 2> [287]
                                   1             feResource(feResourceConfig_s *, int) <cycle 2> [236]
-----------------------------------------------
                0.00    0.00       6/6           feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
[283]    0.0    0.00    0.00       6         feVerifyResourceValue(feResourceType, char *) [283]
-----------------------------------------------
                0.00    0.00       1/6           jjJACOB_P(sleftv *, sleftv *) [154]
                0.00    0.00       1/6           idCopy(sip_sideal *) [99]
                0.00    0.00       1/6           iiAssign(sleftv *, sleftv *) [88]
                0.00    0.00       1/6           initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
                0.00    0.00       2/6           idrec::set(char *, int, int, short) [149]
[284]    0.0    0.00    0.00       6         idInit(int, int) [284]
-----------------------------------------------
                0.00    0.00       6/6           enterT(sLObject, skStrategy *, int) [33]
[285]    0.0    0.00    0.00       6         posInT2(sTObject *, int, sLObject const &) [285]
-----------------------------------------------
                0.00    0.00       5/5           iiGetLibProcBuffer(procinfo *, int) [141]
[286]    0.0    0.00    0.00       5         iiProcArgs(char *, short) [286]
-----------------------------------------------
                                   4             feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
[287]    0.0    0.00    0.00       4         feSprintf(char *, char const *, int) <cycle 2> [287]
                0.00    0.00       7/20          feGetResourceConfig(char) [235]
                                   7             feResource(feResourceConfig_s *, int) <cycle 2> [236]
-----------------------------------------------
                0.00    0.00       2/4           iiLibCmd(char *, short) [59]
                0.00    0.00       2/4           iiExport(sleftv *, int) [326]
[288]    0.0    0.00    0.00       4         idrec::get(char const *, int) [288]
-----------------------------------------------
                0.00    0.00       4/4           iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) <cycle 1> [83]
[289]    0.0    0.00    0.00       4         jjINDEX_I(sleftv *, sleftv *, sleftv *) [289]
                0.00    0.00       4/4           jjMakeSub(sleftv *) [290]
-----------------------------------------------
                0.00    0.00       4/4           jjINDEX_I(sleftv *, sleftv *, sleftv *) [289]
[290]    0.0    0.00    0.00       4         jjMakeSub(sleftv *) [290]
                0.00    0.00       4/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00       4/4           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[291]    0.0    0.00    0.00       4         make_version(char *, int) [291]
-----------------------------------------------
                0.00    0.00       4/4           feCleanUpFile(char *) [221]
[292]    0.0    0.00    0.00       4         mystrcpy(char *, char *) [292]
-----------------------------------------------
                0.00    0.00       4/4           reinit_yylp(void) [310]
[293]    0.0    0.00    0.00       4         yy_flex_free(void *) [293]
                0.00    0.00       2/70          omSizeOfLargeAddr [198]
                0.00    0.00       2/41          omFreeSizeToSystem [211]
-----------------------------------------------
                0.00    0.00       4/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [295]
[294]    0.0    0.00    0.00       4         yylp_flush_buffer(yy_buffer_state *) [294]
-----------------------------------------------
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [311]
                0.00    0.00       2/4           yylp_create_buffer(_IO_FILE *, int) [118]
[295]    0.0    0.00    0.00       4         yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [295]
                0.00    0.00       4/4           yylp_flush_buffer(yy_buffer_state *) [294]
-----------------------------------------------
                0.00    0.00       2/4           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
                0.00    0.00       2/4           yylprestart(_IO_FILE *) [311]
[296]    0.0    0.00    0.00       4         yylp_load_buffer_state(void) [296]
-----------------------------------------------
                0.00    0.00       3/3           opendir [134]
[297]    0.0    0.00    0.00       3         calloc [297]
-----------------------------------------------
                0.00    0.00       3/3           omUpdateInfo [76]
[298]    0.0    0.00    0.00       3         malloc_update_mallinfo [298]
-----------------------------------------------
                0.00    0.00       2/2           FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [250]
[299]    0.0    0.00    0.00       2         ZeroOrd_2_NonZeroOrd(p_Ord, int) [299]
-----------------------------------------------
                0.00    0.00       2/2           feCleanResourceValue(feResourceType, char *) [281]
[300]    0.0    0.00    0.00       2         feCleanUpPath(char *) [300]
                0.00    0.00      29/33          feCleanUpFile(char *) [221]
-----------------------------------------------
                0.00    0.00       2/2           jjSYSTEM(sleftv *, sleftv *) [234]
[301]    0.0    0.00    0.00       2         feGetOptIndex(char const *) [301]
-----------------------------------------------
                0.00    0.00       2/2           fe_reset_input_mode(void) [303]
[302]    0.0    0.00    0.00       2         fe_reset_fe [302]
-----------------------------------------------
                0.00    0.00       2/2           m2_end [338]
[303]    0.0    0.00    0.00       2         fe_reset_input_mode(void) [303]
                0.00    0.00       2/2           fe_reset_fe [302]
-----------------------------------------------
                0.00    0.00       2/2           __gmpf_clear [4797]
[304]    0.0    0.00    0.00       2         freeSize [304]
-----------------------------------------------
                0.00    0.00       2/2           sleftv::Data(void) [166]
[305]    0.0    0.00    0.00       2         getTimer(void) [305]
-----------------------------------------------
                0.00    0.00       1/2           firstUpdate(skStrategy *) [28]
                0.00    0.00       1/2           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[306]    0.0    0.00    0.00       2         kMoraUseBucket(skStrategy *) [306]
-----------------------------------------------
                0.00    0.00       1/2           global constructors keyed to gmp_output_digits [4329]
                0.00    0.00       1/2           initCanonicalForm(void) [1739]
[307]    0.0    0.00    0.00       2         mmInit(void) [307]
                0.00    0.00       1/1           omInitInfo [345]
-----------------------------------------------
                0.00    0.00       2/2           omDoRealloc [125]
[308]    0.0    0.00    0.00       2         omSizeOfAddr [308]
                0.00    0.00       1/71          malloc_usable_size [197]
-----------------------------------------------
                0.00    0.00       1/2           idSkipZeroes(sip_sideal *) [324]
                0.00    0.00       1/2           enterSBba(sLObject, int, skStrategy *, int) [213]
[309]    0.0    0.00    0.00       2         pEnlargeSet(spolyrec ***, int, int) [309]
-----------------------------------------------
                0.00    0.00       2/2           iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [60]
[310]    0.0    0.00    0.00       2         reinit_yylp(void) [310]
                0.00    0.00       4/4           yy_flex_free(void *) [293]
-----------------------------------------------
                0.00    0.00       2/2           yy_get_next_buffer(void) [262]
[311]    0.0    0.00    0.00       2         yylprestart(_IO_FILE *) [311]
                0.00    0.00       2/4           yylp_init_buffer(yy_buffer_state *, _IO_FILE *) [295]
                0.00    0.00       2/4           yylp_load_buffer_state(void) [296]
-----------------------------------------------
                0.00    0.00       2/2           yylplex(char *, char *, lib_style_types *, lp_modes) [62]
[312]    0.0    0.00    0.00       2         yylpwrap [312]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [57]
[313]    0.0    0.00    0.00       1         IsPrime(int) [313]
-----------------------------------------------
                0.00    0.00       1/1           sleftv::Print(sleftv *, int) [128]
[314]    0.0    0.00    0.00       1         PrintLn [314]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [355]
[315]    0.0    0.00    0.00       1         SetProcs(p_Field, p_Length, p_Ord) [315]
                0.00    0.00      14/14          FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [250]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[316]    0.0    0.00    0.00       1         atGet(sleftv *, char *) [316]
-----------------------------------------------
                0.00    0.00       1/1           iiAssign(sleftv *, sleftv *) [88]
[317]    0.0    0.00    0.00       1         exprlist_length(sleftv *) [317]
                0.00    0.00       2/1176        sleftv::Typ(void) [161]
                0.00    0.00       1/575         sleftv::Data(void) [166]
-----------------------------------------------
                0.00    0.00       1/1           feInitResource(feResourceConfig_s *, int) <cycle 2> [282]
[318]    0.0    0.00    0.00       1         feGetExpandedExecutable(void) [318]
                0.00    0.00       1/1           omFindExec [341]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[319]    0.0    0.00    0.00       1         feInitResources(char *) [319]
                0.00    0.00       4/20          feGetResourceConfig(char) [235]
                0.00    0.00       4/12          feResource(feResourceConfig_s *, int) <cycle 2> [236]
-----------------------------------------------
                0.00    0.00       1/1           feSetOptValue(feOptIndex, int) [321]
[320]    0.0    0.00    0.00       1         feOptAction(feOptIndex) [320]
-----------------------------------------------
                0.00    0.00       1/1           jjSYSTEM(sleftv *, sleftv *) [234]
[321]    0.0    0.00    0.00       1         feSetOptValue(feOptIndex, int) [321]
                0.00    0.00       1/1           feOptAction(feOptIndex) [320]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[322]    0.0    0.00    0.00       1         fe_getopt_long [322]
                0.00    0.00       1/1           _fe_getopt_internal [4344]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [341]
[323]    0.0    0.00    0.00       1         full_readlink [323]
-----------------------------------------------
                0.00    0.00       1/1           jjSTD(sleftv *, sleftv *) [5]
[324]    0.0    0.00    0.00       1         idSkipZeroes(sip_sideal *) [324]
                0.00    0.00       1/2           pEnlargeSet(spolyrec ***, int, int) [309]
-----------------------------------------------
                0.00    0.00       1/1           iiConvert(int, int, int, sleftv *, sleftv *) [113]
[325]    0.0    0.00    0.00       1         iiDummy(void *) [325]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [39]
[326]    0.0    0.00    0.00       1         iiExport(sleftv *, int) [326]
                0.00    0.00       2/4           idrec::get(char const *, int) [288]
                0.00    0.00       1/988         sleftv::CleanUp(void) [163]
-----------------------------------------------
                0.00    0.00       1/1           iiConvert(int, int, int, sleftv *, sleftv *) [113]
[327]    0.0    0.00    0.00       1         iiI2P(void *) [327]
                0.00    0.00       1/15          p_ISet(int, sip_sring *) [249]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[328]    0.0    0.00    0.00       1         initBuchMoraCrit(skStrategy *) [328]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[329]    0.0    0.00    0.00       1         initBuchMoraPos(skStrategy *) [329]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[330]    0.0    0.00    0.00       1         initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [330]
-----------------------------------------------
                0.00    0.00       1/1           mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [7]
[331]    0.0    0.00    0.00       1         initMora(sip_sideal *, skStrategy *) [331]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [335]
[332]    0.0    0.00    0.00       1         initRTimer(void) [332]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [335]
[333]    0.0    0.00    0.00       1         initTimer(void) [333]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [335]
[334]    0.0    0.00    0.00       1         init_signals(void) [334]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[335]    0.0    0.00    0.00       1         inits(void) [335]
                0.00    0.00       1/1           init_signals(void) [334]
                0.00    0.00       1/1           initTimer(void) [333]
                0.00    0.00       1/1           initRTimer(void) [332]
                0.00    0.00       1/1           mpsr_Init(void) [339]
-----------------------------------------------
                0.00    0.00       1/1           main [1]
[336]    0.0    0.00    0.00       1         jjInitTab1(void) [336]
-----------------------------------------------
                0.00    0.00       1/1           iiExprArithM(sleftv *, sleftv *, int) <cycle 1> [132]
[337]    0.0    0.00    0.00       1         jjOPTION_PL(sleftv *, sleftv *) [337]
                0.00    0.00       1/1           setOption(sleftv *, sleftv *) [370]
-----------------------------------------------
                0.00    0.00       1/1           yylex(MYYSTYPE *) [109]
[338]    0.0    0.00    0.00       1         m2_end [338]
                0.00    0.00       2/2           fe_reset_input_mode(void) [303]
-----------------------------------------------
                0.00    0.00       1/1           inits(void) [335]
[339]    0.0    0.00    0.00       1         mpsr_Init(void) [339]
-----------------------------------------------
                0.00    0.00       1/1           _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [107]
[340]    0.0    0.00    0.00       1         npAdd(snumber *, snumber *) [340]
-----------------------------------------------
                0.00    0.00       1/1           feGetExpandedExecutable(void) [318]
[341]    0.0    0.00    0.00       1         omFindExec [341]
                0.00    0.00       1/1           omFindExec_link [342]
                0.00    0.00       1/1           full_readlink [323]
-----------------------------------------------
                0.00    0.00       1/1           omFindExec [341]
[342]    0.0    0.00    0.00       1         omFindExec_link [342]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [347]
[343]    0.0    0.00    0.00       1         omFreeKeptAddrFromBin [343]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[344]    0.0    0.00    0.00       1         omGetStickyBinOfBin [344]
-----------------------------------------------
                0.00    0.00       1/1           mmInit(void) [307]
[345]    0.0    0.00    0.00       1         omInitInfo [345]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [347]
[346]    0.0    0.00    0.00       1         omIsKnownTopBin [346]
-----------------------------------------------
                0.00    0.00       1/1           kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
[347]    0.0    0.00    0.00       1         omMergeStickyBinIntoBin [347]
                0.00    0.00       1/1           _omIsOnList [4345]
                0.00    0.00       1/1           omIsKnownTopBin [346]
                0.00    0.00       1/1           omFreeKeptAddrFromBin [343]
                0.00    0.00       1/1           _omRemoveFromList [4346]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[348]    0.0    0.00    0.00       1         omSizeWOfAddr [348]
-----------------------------------------------
                0.00    0.00       1/1           initSL(sip_sideal *, sip_sideal *, skStrategy *) [55]
[349]    0.0    0.00    0.00       1         pIsConstant(spolyrec *) [349]
-----------------------------------------------
                0.00    0.00       1/1           jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [140]
[350]    0.0    0.00    0.00       1         pNormalize(spolyrec *) [350]
-----------------------------------------------
                0.00    0.00       1/1           rSetHdl(idrec *, short) [56]
[351]    0.0    0.00    0.00       1         pSetGlobals(sip_sring *, short) [351]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [355]
[352]    0.0    0.00    0.00       1         p_FieldIs(sip_sring *) [352]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[353]    0.0    0.00    0.00       1         p_GetSetmProc(sip_sring *) [353]
-----------------------------------------------
                0.00    0.00       1/1           p_SetProcs(sip_sring *, p_Procs_s *) [355]
[354]    0.0    0.00    0.00       1         p_OrdIs(sip_sring *) [354]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[355]    0.0    0.00    0.00       1         p_SetProcs(sip_sring *, p_Procs_s *) [355]
                0.00    0.00       1/1           p_FieldIs(sip_sring *) [352]
                0.00    0.00       1/1           p_OrdIs(sip_sring *) [354]
                0.00    0.00       1/1           SetProcs(p_Field, p_Length, p_Ord) [315]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
[356]    0.0    0.00    0.00       1         pairs(skStrategy *) [356]
-----------------------------------------------
                0.00    0.00       1/1           nInitChar(sip_sring *) [63]
[357]    0.0    0.00    0.00       1         rFieldType(sip_sring *) [357]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[358]    0.0    0.00    0.00       1         rGetDivMask(int) [358]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[359]    0.0    0.00    0.00       1         rGetExpSize(unsigned long, int &, int) [359]
-----------------------------------------------
                0.00    0.00       1/1           rSetDegStuff(sip_sring *) [363]
[360]    0.0    0.00    0.00       1         rOrd_is_Totaldegree_Ordering(sip_sring *) [360]
-----------------------------------------------
                0.00    0.00       1/1           yyparse(void) <cycle 1> [39]
[361]    0.0    0.00    0.00       1         rOrderName(char *) [361]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[362]    0.0    0.00    0.00       1         rRightAdjustVarOffset(sip_sring *) [362]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[363]    0.0    0.00    0.00       1         rSetDegStuff(sip_sring *) [363]
                0.00    0.00       1/1           rOrd_is_Totaldegree_Ordering(sip_sring *) [360]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[364]    0.0    0.00    0.00       1         rSetNegWeight(sip_sring *) [364]
-----------------------------------------------
                0.00    0.00       1/1           rComplete(sip_sring *, int) [61]
[365]    0.0    0.00    0.00       1         rSetVarL(sip_sring *) [365]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [57]
[366]    0.0    0.00    0.00       1         rSleftvList2StringArray(sleftv *, char **) [366]
-----------------------------------------------
                0.00    0.00       1/1           rInit(sleftv *, sleftv *, sleftv *) [57]
[367]    0.0    0.00    0.00       1         rSleftvOrdering2Ordering(sleftv *, sip_sring *) [367]
-----------------------------------------------
                0.00    0.00       1/1           updateS(short, skStrategy *) [374]
[368]    0.0    0.00    0.00       1         reorderS(int *, skStrategy *) [368]
-----------------------------------------------
                0.00    0.00       1/1           firstUpdate(skStrategy *) [28]
[369]    0.0    0.00    0.00       1         reorderT(skStrategy *) [369]
-----------------------------------------------
                0.00    0.00       1/1           jjOPTION_PL(sleftv *, sleftv *) [337]
[370]    0.0    0.00    0.00       1         setOption(sleftv *, sleftv *) [370]
                0.00    0.00       1/1176        sleftv::Typ(void) [161]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [152]
[371]    0.0    0.00    0.00       1         slInitDBMExtension(s_si_link_extension *) [371]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [152]
[372]    0.0    0.00    0.00       1         slInitMPFileExtension(s_si_link_extension *) [372]
-----------------------------------------------
                0.00    0.00       1/1           slStandardInit(void) [152]
[373]    0.0    0.00    0.00       1         slInitMPTcpExtension(s_si_link_extension *) [373]
-----------------------------------------------
                0.00    0.00       1/1           initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [54]
[374]    0.0    0.00    0.00       1         updateS(short, skStrategy *) [374]
                0.00    0.00       1/1           reorderS(int *, skStrategy *) [368]
-----------------------------------------------
                0.00    0.00     156/156         initPT(void) [121]
[4306]   0.0    0.00    0.00     156         Array<CanonicalForm>::operator[](int) const [4306]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [121]
[4307]   0.0    0.00    0.00      82         Array<CanonicalForm>::~Array(void) [4307]
                0.00    0.00      82/82          __builtin_vec_delete [4308]
-----------------------------------------------
                0.00    0.00      82/82          Array<CanonicalForm>::~Array(void) [4307]
[4308]   0.0    0.00    0.00      82         __builtin_vec_delete [4308]
-----------------------------------------------
                0.00    0.00      82/82          initPT(void) [121]
[4309]   0.0    0.00    0.00      82         Array<CanonicalForm>::Array(void) [4309]
-----------------------------------------------
                0.00    0.00       1/64          yyparse(void) <cycle 1> [39]
                0.00    0.00       1/64          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       2/64          newFile(char *, _IO_FILE *) [145]
                0.00    0.00      60/64          newBuffer(char *, feBufferTypes, procinfo *, int) [85]
[4310]   0.0    0.00    0.00      64         __builtin_new [4310]
-----------------------------------------------
                0.00    0.00       1/63          kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [6]
                0.00    0.00       1/63          sleftv::CleanUp(void) [163]
                0.00    0.00      61/63          exitVoice(void) [206]
[4311]   0.0    0.00    0.00      63         __builtin_delete [4311]
-----------------------------------------------
                0.00    0.00       1/42          global constructors keyed to sattr::Print(void) [4319]
                0.00    0.00       1/42          global constructors keyed to slists_bin [4341]
                0.00    0.00       1/42          global constructors keyed to rnumber_bin [4336]
                0.00    0.00       1/42          global constructors keyed to ip_smatrix_bin [4331]
                0.00    0.00       1/42          global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4342]
                0.00    0.00       1/42          rComplete(sip_sring *, int) [61]
                0.00    0.00       1/42          global constructors keyed to mpsr_sleftv_bin [4334]
                0.00    0.00       1/42          global constructors keyed to kBucketCreate(sip_sring *) [4333]
                0.00    0.00       1/42          global constructors keyed to InternalInteger::InternalInteger_bin [4446]
                0.00    0.00       2/42          global constructors keyed to term::term_bin [4448]
                0.00    0.00       3/42          global constructors keyed to s_si_link_extension_bin [4338]
                0.00    0.00       5/42          global constructors keyed to sSubexpr_bin [4337]
                0.00    0.00       6/42          global constructors keyed to sip_command_bin [4340]
                0.00    0.00      17/42          global constructors keyed to MP_INT_bin [4318]
[4312]   0.0    0.00    0.00      42         _omGetSpecBin [4312]
                0.00    0.00      14/14          _omFindInSortedList [4314]
                0.00    0.00       6/6           _omInsertInSortedList [4315]
-----------------------------------------------
                0.00    0.00      26/26          omAllocNewBinPagesRegion [87]
[4313]   0.0    0.00    0.00      26         _omVallocFromSystem [4313]
-----------------------------------------------
                0.00    0.00      14/14          _omGetSpecBin [4312]
[4314]   0.0    0.00    0.00      14         _omFindInSortedList [4314]
-----------------------------------------------
                0.00    0.00       6/6           _omGetSpecBin [4312]
[4315]   0.0    0.00    0.00       6         _omInsertInSortedList [4315]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4772]
[4316]   0.0    0.00    0.00       1         global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4316]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_dtors_aux [4772]
[4317]   0.0    0.00    0.00       1         global destructors keyed to gmp_output_digits [4317]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4318]   0.0    0.00    0.00       1         global constructors keyed to MP_INT_bin [4318]
                0.00    0.00      17/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4319]   0.0    0.00    0.00       1         global constructors keyed to sattr::Print(void) [4319]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4320]   0.0    0.00    0.00       1         global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [4320]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4321]   0.0    0.00    0.00       1         global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [4321]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4322]   0.0    0.00    0.00       1         global constructors keyed to idealFunctionals::idealFunctionals(int, int) [4322]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4323]   0.0    0.00    0.00       1         global constructors keyed to convSingNClapN(snumber *) [4323]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4324]   0.0    0.00    0.00       1         global constructors keyed to dArith2 [4324]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4325]   0.0    0.00    0.00       1         global constructors keyed to feOptSpec [4325]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4326]   0.0    0.00    0.00       1         global constructors keyed to feVersionId [4326]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4327]   0.0    0.00    0.00       1         global constructors keyed to fglmUpdatesource(sip_sideal *) [4327]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4328]   0.0    0.00    0.00       1         global constructors keyed to fglmcomb.cc [4328]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4329]   0.0    0.00    0.00       1         global constructors keyed to gmp_output_digits [4329]
                0.00    0.00       1/2           mmInit(void) [307]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4330]   0.0    0.00    0.00       1         global constructors keyed to iiCurrArgs [4330]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4331]   0.0    0.00    0.00       1         global constructors keyed to ip_smatrix_bin [4331]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4332]   0.0    0.00    0.00       1         global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [4332]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4333]   0.0    0.00    0.00       1         global constructors keyed to kBucketCreate(sip_sring *) [4333]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4334]   0.0    0.00    0.00       1         global constructors keyed to mpsr_sleftv_bin [4334]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4335]   0.0    0.00    0.00       1         global constructors keyed to omSingOutOfMemoryFunc [4335]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4336]   0.0    0.00    0.00       1         global constructors keyed to rnumber_bin [4336]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4337]   0.0    0.00    0.00       1         global constructors keyed to sSubexpr_bin [4337]
                0.00    0.00       5/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4338]   0.0    0.00    0.00       1         global constructors keyed to s_si_link_extension_bin [4338]
                0.00    0.00       3/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4339]   0.0    0.00    0.00       1         global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [4339]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4340]   0.0    0.00    0.00       1         global constructors keyed to sip_command_bin [4340]
                0.00    0.00       6/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4341]   0.0    0.00    0.00       1         global constructors keyed to slists_bin [4341]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4342]   0.0    0.00    0.00       1         global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [4342]
                0.00    0.00       1/42          _omGetSpecBin [4312]
-----------------------------------------------
                0.00    0.00       1/1           __do_global_ctors_aux [127]
[4343]   0.0    0.00    0.00       1         global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [4343]
-----------------------------------------------
                0.00    0.00       1/1           fe_getopt_long [322]
[4344]   0.0    0.00    0.00       1         _fe_getopt_internal [4344]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [347]
[4345]   0.0    0.00    0.00       1         _omIsOnList [4345]
-----------------------------------------------
                0.00    0.00       1/1           omMergeStickyBinIntoBin [347]
[4346]   0.0    0.00    0.00       1         _omRemoveFromList [4346]
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

 [163] sleftv::CleanUp(void)  [99] idCopy(sip_sideal *)   [87] omAllocNewBinPagesRegion (omBinPage.c)
  [93] sleftv::CopyD(int)    [155] idHomIdeal(sip_sideal *, sip_sideal *) [122] omBinPageIndexFault (omBinPage.c)
 [223] sleftv::Copy(sleftv *) [284] idInit(int, int)     [125] omDoRealloc
 [166] sleftv::Data(void)    [264] idRankFreeModule(sip_sideal *, sip_sring *, sip_sring *) [341] omFindExec
 [265] sleftv::Eval(void)    [324] idSkipZeroes(sip_sideal *) [342] omFindExec_link (omFindExec.c)
 [250] FastProcFilter(p_Proc, p_Field &, p_Length &, p_Ord &) [96] id_Delete(sip_sideal **, sip_sring *) [162] omFreeBinPages
 [220] sLObject::GetpLength(void) [88] iiAssign(sleftv *, sleftv *) [343] omFreeKeptAddrFromBin
 [212] HEckeTest(spolyrec *, skStrategy *) [151] iiCheckNest(void) [211] omFreeSizeToSystem
 [170] IsCmd(char *, int &)  [167] iiCheckRing(int)       [21] omFreeToPageFault
 [313] IsPrime(int)          [225] iiConvName(char *)    [344] omGetStickyBinOfBin
 [192] sleftv::LData(void)   [113] iiConvert(int, int, int, sleftv *, sleftv *) [75] omGetUsedBinBytes
  [73] Print                 [157] iiDeclCommand(sleftv *, sleftv *, int, int, idrec **, short, short) [72] omGetUsedBytesOfBin (omBin.c)
 [314] PrintLn               [325] iiDummy(void *)       [345] omInitInfo
  [69] PrintS                [326] iiExport(sleftv *, int) [346] omIsKnownTopBin
 [128] sleftv::Print(sleftv *, int) [4] iiExprArith1(sleftv *, sleftv *, int) [347] omMergeStickyBinIntoBin
 [315] SetProcs(p_Field, p_Length, p_Ord) [83] iiExprArith2(sleftv *, sleftv *, int, sleftv *, short) [260] omRealloc0Large
 [209] sleftv::String(void *, short, int) [274] iiExprArith3(sleftv *, int, sleftv *, sleftv *, sleftv *) [217] omReallocSizeFromSystem
 [161] sleftv::Typ(void)     [132] iiExprArithM(sleftv *, sleftv *, int) [123] omRegisterBinPages (omBinPage.c)
 [299] ZeroOrd_2_NonZeroOrd(p_Ord, int) [141] iiGetLibProcBuffer(procinfo *, int) [308] omSizeOfAddr
 [4307] Array<CanonicalForm>::~Array(void) [327] iiI2P(void *) [198] omSizeOfLargeAddr
 [4316] global destructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [210] iiInitSingularProcinfo(procinfo *, char *, char *, int, long, short) [348] omSizeWOfAddr
 [4317] global destructors keyed to gmp_output_digits [59] iiLibCmd(char *, short) [76] omUpdateInfo
 [4318] global constructors keyed to MP_INT_bin [60] iiLoadLIB(_IO_FILE *, char *, char *, idrec *, short, short) [165] pCompareChain(spolyrec *, spolyrec *, spolyrec *, spolyrec *)
 [4319] global constructors keyed to sattr::Print(void) [144] iiMake_proc(idrec *, sleftv *) [38] pDeg(spolyrec *, sip_sring *)
 [4320] global constructors keyed to fglmVector::fglmVector(fglmVectorRep *) [92] iiPStart(idrec *, sleftv *) [153] pDiff(spolyrec *, int)
 [4321] global constructors keyed to vandermonde::vandermonde(long, long, long, snumber **, bool) [103] iiParameter(sleftv *) [309] pEnlargeSet(spolyrec ***, int, int)
 [4322] global constructors keyed to idealFunctionals::idealFunctionals(int, int) [286] iiProcArgs(char *, short) [168] pHasNotCF(spolyrec *, spolyrec *)
 [4323] global constructors keyed to convSingNClapN(snumber *) [237] iiS2Link(void *) [349] pIsConstant(spolyrec *)
 [4324] global constructors keyed to dArith2 [179] iiTabIndex(sValCmdTab const *, int, int) [156] pIsHomogeneous(spolyrec *)
 [4325] global constructors keyed to feOptSpec [195] iiTestConvert(int, int) [36] pLDeg0c(spolyrec *, int *, sip_sring *)
 [4326] global constructors keyed to feVersionId [133] iiWRITE(sleftv *, sleftv *) [169] pLcm(spolyrec *, spolyrec *, spolyrec *)
 [126] global constructors keyed to fe_promptstr [328] initBuchMoraCrit(skStrategy *) [16] pLength(spolyrec *)
 [4327] global constructors keyed to fglmUpdatesource(sip_sideal *) [329] initBuchMoraPos(skStrategy *) [25] pNorm(spolyrec *)
 [4328] global constructors keyed to fglmcomb.cc [54] initBuchMora(sip_sideal *, sip_sideal *, skStrategy *) [350] pNormalize(spolyrec *)
 [4329] global constructors keyed to gmp_output_digits [90] initEcartNormal(sLObject *) [91] pPower(spolyrec *, int)
 [4330] global constructors keyed to iiCurrArgs [119] initEcartPairMora(sLObject *, spolyrec *, spolyrec *, int, int) [351] pSetGlobals(sip_sring *, short)
 [4331] global constructors keyed to ip_smatrix_bin [330] initHilbCrit(sip_sideal *, sip_sideal *, intvec **, skStrategy *) [12] p_Add_q__FieldZp_LengthThree_OrdNomogPos__FP8spolyrecT0RiP9sip_sring
 [4332] global constructors keyed to jjSYSTEM(sleftv *, sleftv *) [117] initL(void) [35] p_Copy__FieldZp_LengthFour_OrdGeneral__FP8spolyrecP9sip_sring
 [4333] global constructors keyed to kBucketCreate(sip_sring *) [331] initMora(sip_sideal *, skStrategy *) [40] p_Delete__FieldZp_LengthGeneral_OrdGeneral__FPP8spolyrecP9sip_sring
 [4334] global constructors keyed to mpsr_sleftv_bin [332] initRTimer(void) [352] p_FieldIs(sip_sring *)
 [4335] global constructors keyed to omSingOutOfMemoryFunc [55] initSL(sip_sideal *, sip_sideal *, skStrategy *) [353] p_GetSetmProc(sip_sring *)
 [4336] global constructors keyed to rnumber_bin [333] initTimer(void) [27] p_GetShortExpVector(spolyrec *, sip_sring *)
 [4337] global constructors keyed to sSubexpr_bin [334] init_signals(void) [249] p_ISet(int, sip_sring *)
 [4338] global constructors keyed to s_si_link_extension_bin [101] initenterpairs(spolyrec *, int, int, int, skStrategy *, int) [214] p_IsPurePower(spolyrec *, sip_sring *)
 [4339] global constructors keyed to singclap_gcd(spolyrec *, spolyrec *) [335] inits(void) [181] p_LmCmp(spolyrec *, spolyrec *, sip_sring *)
 [4340] global constructors keyed to sip_command_bin [226] ipMoveId(idrec *) [186] p_MaxComp(spolyrec *, sip_sring *, sip_sring *)
 [4341] global constructors keyed to slists_bin [106] jiA_IDEAL(sleftv *, sleftv *, _ssubexpr *) [11] p_Minus_mm_Mult_qq__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiT0P9sip_sringRP8spolyrec
 [4342] global constructors keyed to smRingCopy(sip_sideal *, sip_sring **, sip_sring &) [233] jiA_INT(sleftv *, sleftv *, _ssubexpr *) [261] p_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sring
 [4343] global constructors keyed to tmax(CanonicalForm const &, CanonicalForm const &) [140] jiA_POLY(sleftv *, sleftv *, _ssubexpr *) [354] p_OrdIs(sip_sring *)
  [78] _IO_do_write          [105] jiA_STRING(sleftv *, sleftv *, _ssubexpr *) [355] p_SetProcs(sip_sring *, p_Procs_s *)
  [79] _IO_file_sync          [95] jiAssign_1(sleftv *, sleftv *) [160] p_Setm_TotalDegree(spolyrec *, sip_sring *)
  [80] _IO_file_write        [254] jjA_L_LIST(sleftv *, sleftv *) [34] p_ShallowCopyDelete__FieldGeneral_LengthGeneral_OrdGeneral__FP8spolyrecP9sip_sringP7omBin_s
  [42] _IO_file_xsputn       [275] jjBRACK_S(sleftv *, sleftv *, sleftv *, sleftv *) [22] p_kBucketSetLm__FieldZp_LengthFour_OrdNomogPosZero__FP7kBucket
  [81] _IO_link_in           [238] jjCOUNT_L(sleftv *, sleftv *) [356] pairs(skStrategy *)
 [130] Array<CanonicalForm>::operator=(Array<CanonicalForm> const &) [248] jjDEFINED(sleftv *, sleftv *) [58] pmInit(char *, short &)
 [4311] __builtin_delete     [239] jjGT_I(sleftv *, sleftv *, sleftv *) [230] namerec::pop(short)
 [4310] __builtin_new        [289] jjINDEX_I(sleftv *, sleftv *, sleftv *) [37] posInL17(sLObject *, int, sLObject *, skStrategy *)
 [4308] __builtin_vec_delete [336] jjInitTab1(void)      [216] posInS(spolyrec **, int, spolyrec *)
 [120] __builtin_vec_new     [154] jjJACOB_P(sleftv *, sleftv *) [187] posInT17(sTObject *, int, sLObject const &)
 (4918) __mcount_internal    [240] jjKLAMMER_PL(sleftv *, sleftv *) [285] posInT2(sTObject *, int, sLObject const &)
 [4309] Array<CanonicalForm>::Array(void) [74] jjMEMORY(sleftv *, sleftv *) [15] pp_Mult_mm_Noether__FieldZp_LengthFour_OrdNomogPosZero__FP8spolyrecN20RiP9sip_sringRP8spolyrec
 [131] Array<CanonicalForm>::Array(int) [251] jjMINUS_I(sleftv *, sleftv *, sleftv *) [32] pp_Mult_mm__FieldZp_LengthFour_OrdGeneral__FP8spolyrecT0P9sip_sringRP8spolyrec
 [4306] Array<CanonicalForm>::operator[](int) const [290] jjMakeSub(sleftv *) [150] namerec::push(sip_package *, char *, int, short)
 [4344] _fe_getopt_internal  [337] jjOPTION_PL(sleftv *, sleftv *) [61] rComplete(sip_sring *, int)
 [4314] _omFindInSortedList  [252] jjOP_REST(sleftv *, sleftv *, sleftv *) [77] rDBTest(sip_sring *, char *, int)
 [4312] _omGetSpecBin        [190] jjPLUSMINUS_Gen(sleftv *, sleftv *, sleftv *) [218] rEALLOc
 [4315] _omInsertInSortedList [102] jjPLUS_P(sleftv *, sleftv *, sleftv *) [357] rFieldType(sip_sring *)
 [4345] _omIsOnList          [194] jjPLUS_S(sleftv *, sleftv *, sleftv *) [358] rGetDivMask(int)
 [4346] _omRemoveFromList     [89] jjPOWER_P(sleftv *, sleftv *, sleftv *) [359] rGetExpSize(unsigned long, int &, int)
 [4313] _omVallocFromSystem  [253] jjPROC1(sleftv *, sleftv *) [57] rInit(sleftv *, sleftv *, sleftv *)
 [107] _p_Mult_q(spolyrec *, spolyrec *, int, sip_sring *) [241] jjPROC(sleftv *, sleftv *, sleftv *) [171] rIsPolyVar(int, sip_sring *)
 [316] atGet(sleftv *, char *) [276] jjREAD(sleftv *, sleftv *) [189] rIsRingVar(char *)
 [297] calloc                  [5] jjSTD(sleftv *, sleftv *) [360] rOrd_is_Totaldegree_Ordering(sip_sring *)
 [180] cancelunit(sLObject *) [222] jjSTRING_PL(sleftv *, sleftv *) [361] rOrderName(char *)
 [111] chainCrit(spolyrec *, int, skStrategy *) [234] jjSYSTEM(sleftv *, sleftv *) [362] rRightAdjustVarOffset(sip_sring *)
  [44] cleanT(skStrategy *)   [97] jjTIMES_P(sleftv *, sleftv *, sleftv *) [363] rSetDegStuff(sip_sring *)
 [175] copy_string(lp_modes)  [20] kBucketCanonicalize(kBucket *) [56] rSetHdl(idrec *, short)
 [172] current_pos(int)       [30] kBucketClear(kBucket *, spolyrec **, int *) [364] rSetNegWeight(sip_sring *)
  [17] deleteHC(sLObject *, skStrategy *, short) [147] kBucketCreate(sip_sring *) [365] rSetVarL(sip_sring *)
 [146] deleteInL(sLObject *, int *, int, skStrategy *) [143] kBucketDestroy(kBucket **) [366] rSleftvList2StringArray(sleftv *, char **)
  [14] doRed(sLObject *, sTObject *, short, skStrategy *) [173] kBucketInit(kBucket *, spolyrec *, int) [367] rSleftvOrdering2Ordering(sleftv *, sip_sring *)
  [67] enterL(sLObject **, int *, int *, sLObject, int) [9] kBucket_Minus_m_Mult_p(kBucket *, spolyrec *, spolyrec *, int *, spolyrec *) [13] redEcart(sLObject *, skStrategy *)
 [104] enterOnePair(int, spolyrec *, int, int, skStrategy *, int) [70] kFindDivisibleByInS(spolyrec **const &, unsigned long const *, int, sLObject *) [10] redFirst(sLObject *, skStrategy *)
 [213] enterSBba(sLObject, int, skStrategy *, int) [19] kFindDivisibleByInT(sTObject *const &, unsigned long const *, int, sLObject const *, int) [215] redtail(spolyrec *, int, skStrategy *)
  [18] enterSMora(sLObject, int, skStrategy *, int) [306] kMoraUseBucket(skStrategy *) [310] reinit_yylp(void)
  [33] enterT(sLObject, skStrategy *, int) [6] kStd(sip_sideal *, sip_sideal *, tHomog, intvec **, intvec *, int, int, intvec *) [108] reorderL(skStrategy *)
 [148] enterid(char *, int, int, idrec **, short) [200] killhdl(idrec *, idrec **) [368] reorderS(int *, skStrategy *)
 [100] enterpairs(spolyrec *, int, int, int, skStrategy *, int) [227] killlocals(int) [369] reorderT(skStrategy *)
  [43] exitBuchMora(skStrategy *) [135] ksCheckCoeff(snumber **, snumber **) [142] scComputeHC(sip_sideal *, int, spolyrec *&)
 [224] exitBuffer(feBufferTypes) [136] ksCreateShortSpoly(spolyrec *, spolyrec *, sip_sring *) [370] setOption(sleftv *, sleftv *)
 [206] exitVoice(void)        [82] ksCreateSpoly(sLObject *, spolyrec *, int, sip_sring *, spolyrec *, spolyrec *, sTObject **) [149] idrec::set(char *, int, int, short)
 [317] exprlist_length(sleftv *) [8] ksReducePoly(sLObject *, sTObject *, spolyrec *, snumber **, skStrategy *) [242] slCleanUp(sip_link *)
 [199] fREe                  [258] lRingDependend(slists *) [243] slCloseAscii(sip_link *)
 [281] feCleanResourceValue(feResourceType, char *) [259] libread(_IO_FILE *, char *, int) [371] slInitDBMExtension(s_si_link_extension *)
 [221] feCleanUpFile(char *) [176] sleftv::listLength(void) [372] slInitMPFileExtension(s_si_link_extension *)
 [300] feCleanUpPath(char *) [338] m2_end                [373] slInitMPTcpExtension(s_si_link_extension *)
 [139] feFopen(char *, char *, char *, int, int) [193] mALLOc [244] slInit(sip_link *, char *)
 [318] feGetExpandedExecutable(void) [1] main             [94] slInternalCopy(sleftv *, int, void *, _ssubexpr *)
 [301] feGetOptIndex(char const *) [291] make_version(char *, int) [245] slKill(sip_link *)
 [235] feGetResourceConfig(char) [110] malloc            [246] slOpenAscii(sip_link *, short)
 [282] feInitResource(feResourceConfig_s *, int) [257] malloc_extend_top (omMalloc.c) [277] slReadAscii2(sip_link *, sleftv *)
 [319] feInitResources(char *) [298] malloc_update_mallinfo [278] slReadAscii(sip_link *)
 [320] feOptAction(feOptIndex) [197] malloc_usable_size  [279] slRead(sip_link *, sleftv *)
 [174] fePrintEcho(char *, char *) (2377) mcount         [152] slStandardInit(void)
 [114] feReadLine(char *, int) [53] memmove              [255] slWriteAscii(sip_link *, sleftv *)
 [236] feResource(feResourceConfig_s *, int) [129] messageStat(int, int, int, skStrategy *) [256] slWrite(sip_link *, sleftv *)
 [263] feResource(char, int)  [48] message(int, int *, int *, skStrategy *) [49] syMake(sleftv *, char *, idrec *)
 [321] feSetOptValue(feOptIndex, int) [307] mmInit(void)  [26] updateLHC(skStrategy *)
 [287] feSprintf(char *, char const *, int) [7] mora(sip_sideal *, sip_sideal *, intvec *, intvec *, skStrategy *) [374] updateS(short, skStrategy *)
 [283] feVerifyResourceValue(feResourceType, char *) [339] mpsr_Init(void) [29] updateT(skStrategy *)
 [322] fe_getopt_long        [232] myfread(void *, unsigned int, unsigned int, _IO_FILE *) [24] write
 [302] fe_reset_fe           [292] mystrcpy(char *, char *) [124] yy_create_buffer(_IO_FILE *, int)
 [303] fe_reset_input_mode(void) [86] myynewbuffer(void)  [84] yy_flex_alloc(unsigned int)
  [28] firstUpdate(skStrategy *) [228] myyoldbuffer(void *) [116] yy_flex_alloc(unsigned int)
  [51] fork                   [45] nDummy1(snumber **)   [205] yy_flex_free(void *)
 [219] free                   [50] nDummy2(snumber *&)   [293] yy_flex_free(void *)
 [304] freeSize               [63] nInitChar(sip_sring *) [203] yy_flush_buffer(yy_buffer_state *)
 [323] full_readlink (omFindExec.c) [64] nSetChar(sip_sring *, short) [115] yy_get_next_buffer(void)
  [52] fwrite                [188] ndCopy(snumber *)     [262] yy_get_next_buffer(void)
 [305] getTimer(void)        [196] ndGcd(snumber *, snumber *) [164] yy_get_previous_state(void)
 [288] idrec::get(char const *, int) [85] newBuffer(char *, feBufferTypes, procinfo *, int) [280] yy_get_previous_state(void)
  [71] ggetid(char const *, short) [145] newFile(char *, _IO_FILE *) [204] yy_init_buffer(yy_buffer_state *, _IO_FILE *)
 [266] hCreate(int)           [98] newHEdge(spolyrec **, int, skStrategy *) [191] yy_load_buffer_state(void)
 [267] hDelete(long **, int) [340] npAdd(snumber *, snumber *) [109] yylex(MYYSTYPE *)
 [184] hElimS(long **, int *, int, int, int *, int) [46] npDiv(snumber *, snumber *) [118] yylp_create_buffer(_IO_FILE *, int)
 [201] hGetmem(int, long **, monrec *) [247] npEqual(snumber *, snumber *) [294] yylp_flush_buffer(yy_buffer_state *)
 [202] hGetpure(long *)       [65] npInitChar(int, sip_sring *) [295] yylp_init_buffer(yy_buffer_state *, _IO_FILE *)
 [268] hHedgeStep(long *, long **, int, int *, int, spolyrec *) [177] npInit(int) [296] yylp_load_buffer_state(void)
 [182] hHedge(spolyrec *)     [41] npIsOne(snumber *)     [62] yylplex(char *, char *, lib_style_types *, lp_modes)
 [269] hInit(sip_sideal *, sip_sideal *, int *, sip_sring *) [229] npIsZero(snumber *) [311] yylprestart(_IO_FILE *)
 [270] hKill(monrec **, int) [207] npMult(snumber *, snumber *) [312] yylpwrap
 [185] hLex2S(long **, int, int, int, int *, int, long **) [47] npNeg(snumber *) [39] yyparse(void)
 [271] hLexS(long **, int, int *, int) [208] npRead(char *, snumber **) [231] yyrestart(_IO_FILE *)
 [272] hOrdSupp(long **, int, int *, int) [66] npSetChar(int, sip_sring *) [3] <cycle 1>
 [183] hPure(long **, int, int *, int *, int, long *, int *) [23] omAllocBinFromFullPage [159] <cycle 2>
 [273] hStaircase(long **, int *, int *, int) [31] omAllocBinPage
 [178] hStepS(long **, int, int *, int, int *, long *) [68] omAllocFromSystem
