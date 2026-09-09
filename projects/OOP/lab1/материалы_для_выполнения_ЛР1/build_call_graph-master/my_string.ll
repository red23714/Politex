; ModuleID = '../../src/my_string/my_string.cpp'
source_filename = "../../src/my_string/my_string.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

module asm ".globl _ZSt21ios_base_library_initv"

%"class.std::basic_ostream" = type { ptr, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", ptr, i8, i8, ptr, ptr, ptr, ptr }
%"class.std::ios_base" = type { ptr, i64, i64, i32, i32, i32, ptr, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, ptr, %"class.std::locale" }
%"struct.std::ios_base::_Words" = type { ptr, i64 }
%"class.std::locale" = type { ptr }
%"class.std::basic_string_view" = type { i64, ptr }
%class.MyString = type { ptr, i32, i32 }
%"class.std::__cxx11::basic_string" = type { %"struct.std::__cxx11::basic_string<char>::_Alloc_hider", i64, %union.anon.0 }
%"struct.std::__cxx11::basic_string<char>::_Alloc_hider" = type { ptr }
%union.anon.0 = type { i64, [8 x i8] }

$_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv = comdat any

$_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv = comdat any

$_ZNKSt17basic_string_viewIcSt11char_traitsIcEE6substrEmm = comdat any

$_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKc = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEcvSt17basic_string_viewIcS2_EEv = comdat any

$_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKcm = comdat any

$_ZSt10__sv_checkmmPKc = comdat any

$_ZSt3minImERKT_S2_S2_ = comdat any

$_ZNSt11char_traitsIcE6lengthEPKc = comdat any

$__clang_call_terminate = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4dataEv = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4sizeEv = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEv = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE8max_sizeEv = comdat any

$_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_get_allocatorEv = comdat any

@.str = private unnamed_addr constant [38 x i8] c"Index is bigger than length of string\00", align 1
@_ZTISt12out_of_range = external constant ptr
@_ZSt4cout = external global %"class.std::basic_ostream", align 8
@.str.1 = private unnamed_addr constant [2 x i8] c"\22\00", align 1
@.str.2 = private unnamed_addr constant [2 x i8] c" \00", align 1
@.str.3 = private unnamed_addr constant [2 x i8] c"(\00", align 1
@.str.4 = private unnamed_addr constant [3 x i8] c", \00", align 1
@.str.5 = private unnamed_addr constant [2 x i8] c")\00", align 1
@.str.6 = private unnamed_addr constant [48 x i8] c"Substring count is larger than length of string\00", align 1
@.str.7 = private unnamed_addr constant [26 x i8] c"basic_string_view::substr\00", align 1
@.str.8 = private unnamed_addr constant [49 x i8] c"%s: __pos (which is %zu) > __size (which is %zu)\00", align 1

@_ZN8MyStringC1Ev = dso_local unnamed_addr alias void (ptr), ptr @_ZN8MyStringC2Ev
@_ZN8MyStringC1ESt17basic_string_viewIcSt11char_traitsIcEE = dso_local unnamed_addr alias void (ptr, i64, ptr), ptr @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEE
@_ZN8MyStringC1ESt17basic_string_viewIcSt11char_traitsIcEEi = dso_local unnamed_addr alias void (ptr, i64, ptr, i32), ptr @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEEi
@_ZN8MyStringC1Eic = dso_local unnamed_addr alias void (ptr, i32, i8), ptr @_ZN8MyStringC2Eic
@_ZN8MyStringC1ERKS_ = dso_local unnamed_addr alias void (ptr, ptr), ptr @_ZN8MyStringC2ERKS_
@_ZN8MyStringC1EPKc = dso_local unnamed_addr alias void (ptr, ptr), ptr @_ZN8MyStringC2EPKc
@_ZN8MyStringC1ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE = dso_local unnamed_addr alias void (ptr, ptr), ptr @_ZN8MyStringC2ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
@_ZN8MyStringC1EOS_ = dso_local unnamed_addr alias void (ptr, ptr), ptr @_ZN8MyStringC2EOS_
@_ZN8MyStringC1Ei = dso_local unnamed_addr alias void (ptr, i32), ptr @_ZN8MyStringC2Ei
@_ZN8MyStringC1Ef = dso_local unnamed_addr alias void (ptr, float), ptr @_ZN8MyStringC2Ef
@_ZN8MyStringD1Ev = dso_local unnamed_addr alias void (ptr), ptr @_ZN8MyStringD2Ev

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %6, align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %7, align 8
  store ptr %0, ptr %5, align 8
  %8 = load ptr, ptr %5, align 8
  %9 = call noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %4) #12
  %10 = trunc i64 %9 to i32
  %11 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 1
  store i32 %10, ptr %11, align 8
  %12 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 1
  %13 = load i32, ptr %12, align 8
  %14 = add nsw i32 %13, 1
  %15 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 2
  store i32 %14, ptr %15, align 4
  %16 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 2
  %17 = load i32, ptr %16, align 4
  %18 = sext i32 %17 to i64
  %19 = call noalias noundef nonnull ptr @_Znam(i64 noundef %18) #13
  %20 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  store ptr %19, ptr %20, align 8
  %21 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  %22 = load ptr, ptr %21, align 8
  %23 = call noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %4) #12
  %24 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 1
  %25 = load i32, ptr %24, align 8
  %26 = sext i32 %25 to i64
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %22, ptr align 1 %23, i64 %26, i1 false)
  %27 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  %28 = load ptr, ptr %27, align 8
  %29 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 1
  %30 = load i32, ptr %29, align 8
  %31 = sext i32 %30 to i64
  %32 = getelementptr inbounds i8, ptr %28, i64 %31
  store i8 0, ptr %32, align 1
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %3, i32 0, i32 0
  %5 = load i64, ptr %4, align 8
  ret i64 %5
}

; Function Attrs: nobuiltin allocsize(0)
declare noalias noundef nonnull ptr @_Znam(i64 noundef) #2

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %3, i32 0, i32 1
  %5 = load ptr, ptr %4, align 8
  ret ptr %5
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias writeonly captures(none), ptr noalias readonly captures(none), i64, i1 immarg) #3

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef ptr @_ZN8MyString19create_copy_of_pstrEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1) #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %6 = load ptr, ptr %3, align 8
  %7 = load i32, ptr %4, align 4
  %8 = sext i32 %7 to i64
  %9 = call noalias noundef nonnull ptr @_Znam(i64 noundef %8) #13
  store ptr %9, ptr %5, align 8
  %10 = load i32, ptr %4, align 4
  %11 = getelementptr inbounds nuw %class.MyString, ptr %6, i32 0, i32 2
  %12 = load i32, ptr %11, align 4
  %13 = icmp slt i32 %10, %12
  br i1 %13, label %14, label %20

14:                                               ; preds = %2
  %15 = load ptr, ptr %5, align 8
  %16 = getelementptr inbounds nuw %class.MyString, ptr %6, i32 0, i32 0
  %17 = load ptr, ptr %16, align 8
  %18 = load i32, ptr %4, align 4
  %19 = sext i32 %18 to i64
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %15, ptr align 1 %17, i64 %19, i1 false)
  br label %27

20:                                               ; preds = %2
  %21 = load ptr, ptr %5, align 8
  %22 = getelementptr inbounds nuw %class.MyString, ptr %6, i32 0, i32 0
  %23 = load ptr, ptr %22, align 8
  %24 = getelementptr inbounds nuw %class.MyString, ptr %6, i32 0, i32 2
  %25 = load i32, ptr %24, align 4
  %26 = sext i32 %25 to i64
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %21, ptr align 1 %23, i64 %26, i1 false)
  br label %27

27:                                               ; preds = %20, %14
  %28 = load ptr, ptr %5, align 8
  ret ptr %28
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef %1, i32 noundef %2, i32 noundef %3) #1 align 2 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store ptr %0, ptr %5, align 8
  store ptr %1, ptr %6, align 8
  store i32 %2, ptr %7, align 4
  store i32 %3, ptr %8, align 4
  %9 = load ptr, ptr %5, align 8
  %10 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  %11 = load ptr, ptr %10, align 8
  %12 = icmp eq ptr %11, null
  br i1 %12, label %14, label %13

13:                                               ; preds = %4
  call void @_ZdaPv(ptr noundef %11) #14
  br label %14

14:                                               ; preds = %13, %4
  %15 = load i32, ptr %7, align 4
  %16 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 1
  store i32 %15, ptr %16, align 8
  %17 = load i32, ptr %8, align 4
  %18 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 2
  store i32 %17, ptr %18, align 4
  %19 = load ptr, ptr %6, align 8
  %20 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  store ptr %19, ptr %20, align 8
  ret void
}

; Function Attrs: nobuiltin nounwind
declare void @_ZdaPv(ptr noundef) #4

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString9my_insertEiiPKc(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2, ptr noundef %3) #0 align 2 personality ptr @__gxx_personality_v0 {
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  %9 = alloca ptr, align 8
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca ptr, align 8
  store ptr %0, ptr %5, align 8
  store i32 %1, ptr %6, align 4
  store i32 %2, ptr %7, align 4
  store ptr %3, ptr %8, align 8
  %13 = load ptr, ptr %5, align 8
  %14 = load i32, ptr %6, align 4
  %15 = icmp slt i32 %14, 0
  br i1 %15, label %16, label %21

16:                                               ; preds = %4
  %17 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  %18 = load i32, ptr %17, align 4
  %19 = load i32, ptr %6, align 4
  %20 = sub nsw i32 %18, %19
  store i32 %20, ptr %6, align 4
  br label %21

21:                                               ; preds = %16, %4
  %22 = load i32, ptr %6, align 4
  %23 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  %24 = load i32, ptr %23, align 4
  %25 = icmp sgt i32 %22, %24
  br i1 %25, label %29, label %26

26:                                               ; preds = %21
  %27 = load i32, ptr %6, align 4
  %28 = icmp slt i32 %27, 0
  br i1 %28, label %29, label %36

29:                                               ; preds = %26, %21
  %30 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %30, ptr noundef @.str)
          to label %31 unwind label %32

31:                                               ; preds = %29
  call void @__cxa_throw(ptr %30, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

32:                                               ; preds = %29
  %33 = landingpad { ptr, i32 }
          cleanup
  %34 = extractvalue { ptr, i32 } %33, 0
  store ptr %34, ptr %9, align 8
  %35 = extractvalue { ptr, i32 } %33, 1
  store i32 %35, ptr %10, align 4
  call void @__cxa_free_exception(ptr %30) #12
  br label %97

36:                                               ; preds = %26
  %37 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  %38 = load i32, ptr %37, align 4
  %39 = icmp eq i32 %38, 0
  br i1 %39, label %40, label %48

40:                                               ; preds = %36
  %41 = call noalias noundef nonnull ptr @_Znam(i64 noundef 1) #13
  %42 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 0
  store ptr %41, ptr %42, align 8
  %43 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 0
  %44 = load ptr, ptr %43, align 8
  %45 = getelementptr inbounds i8, ptr %44, i64 0
  store i8 0, ptr %45, align 1
  %46 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  store i32 0, ptr %46, align 8
  %47 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  store i32 1, ptr %47, align 4
  br label %48

48:                                               ; preds = %40, %36
  %49 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %50 = load i32, ptr %49, align 8
  %51 = load i32, ptr %7, align 4
  %52 = add nsw i32 %50, %51
  %53 = add nsw i32 %52, 1
  store i32 %53, ptr %11, align 4
  %54 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %55 = load i32, ptr %54, align 8
  %56 = load i32, ptr %7, align 4
  %57 = add nsw i32 %55, %56
  %58 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  %59 = load i32, ptr %58, align 4
  %60 = icmp slt i32 %57, %59
  br i1 %60, label %61, label %64

61:                                               ; preds = %48
  %62 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 2
  %63 = load i32, ptr %62, align 4
  store i32 %63, ptr %11, align 4
  br label %64

64:                                               ; preds = %61, %48
  %65 = load i32, ptr %11, align 4
  %66 = call noundef ptr @_ZN8MyString19create_copy_of_pstrEi(ptr noundef nonnull align 8 dereferenceable(16) %13, i32 noundef %65)
  store ptr %66, ptr %12, align 8
  %67 = load ptr, ptr %12, align 8
  %68 = load i32, ptr %6, align 4
  %69 = sext i32 %68 to i64
  %70 = getelementptr inbounds i8, ptr %67, i64 %69
  %71 = load i32, ptr %7, align 4
  %72 = sext i32 %71 to i64
  %73 = getelementptr inbounds i8, ptr %70, i64 %72
  %74 = load ptr, ptr %12, align 8
  %75 = load i32, ptr %6, align 4
  %76 = sext i32 %75 to i64
  %77 = getelementptr inbounds i8, ptr %74, i64 %76
  %78 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %79 = load i32, ptr %78, align 8
  %80 = add nsw i32 %79, 1
  %81 = load i32, ptr %6, align 4
  %82 = sub nsw i32 %80, %81
  %83 = sext i32 %82 to i64
  call void @llvm.memmove.p0.p0.i64(ptr align 1 %73, ptr align 1 %77, i64 %83, i1 false)
  %84 = load ptr, ptr %12, align 8
  %85 = load i32, ptr %6, align 4
  %86 = sext i32 %85 to i64
  %87 = getelementptr inbounds i8, ptr %84, i64 %86
  %88 = load ptr, ptr %8, align 8
  %89 = load i32, ptr %7, align 4
  %90 = sext i32 %89 to i64
  call void @llvm.memcpy.p0.p0.i64(ptr align 1 %87, ptr align 1 %88, i64 %90, i1 false)
  %91 = load ptr, ptr %12, align 8
  %92 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %93 = load i32, ptr %92, align 8
  %94 = load i32, ptr %7, align 4
  %95 = add nsw i32 %93, %94
  %96 = load i32, ptr %11, align 4
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %13, ptr noundef %91, i32 noundef %95, i32 noundef %96)
  ret void

97:                                               ; preds = %32
  %98 = load ptr, ptr %9, align 8
  %99 = load i32, ptr %10, align 4
  %100 = insertvalue { ptr, i32 } poison, ptr %98, 0
  %101 = insertvalue { ptr, i32 } %100, i32 %99, 1
  resume { ptr, i32 } %101
}

declare ptr @__cxa_allocate_exception(i64)

declare void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16), ptr noundef) unnamed_addr #5

declare i32 @__gxx_personality_v0(...)

declare void @__cxa_free_exception(ptr)

; Function Attrs: nounwind
declare void @_ZNSt12out_of_rangeD1Ev(ptr noundef nonnull align 8 dereferenceable(16)) unnamed_addr #6

declare void @__cxa_throw(ptr, ptr, ptr)

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly captures(none), ptr readonly captures(none), i64, i1 immarg) #3

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString4pstrEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #0 align 2 {
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  %4 = load ptr, ptr %2, align 8
  %5 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.1)
  store i32 0, ptr %3, align 4
  br label %6

6:                                                ; preds = %19, %1
  %7 = load i32, ptr %3, align 4
  %8 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %9 = load i32, ptr %8, align 8
  %10 = icmp slt i32 %7, %9
  br i1 %10, label %11, label %22

11:                                               ; preds = %6
  %12 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 0
  %13 = load ptr, ptr %12, align 8
  %14 = load i32, ptr %3, align 4
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds i8, ptr %13, i64 %15
  %17 = load i8, ptr %16, align 1
  %18 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i8 noundef signext %17)
  br label %19

19:                                               ; preds = %11
  %20 = load i32, ptr %3, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, ptr %3, align 4
  br label %6, !llvm.loop !6

22:                                               ; preds = %6
  %23 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.1)
  %24 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef @.str.2)
  %25 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %24, ptr noundef @.str.3)
  %26 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %27 = load i32, ptr %26, align 8
  %28 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %25, i32 noundef %27)
  %29 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %28, ptr noundef @.str.4)
  %30 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 2
  %31 = load i32, ptr %30, align 4
  %32 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %29, i32 noundef %31)
  %33 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8) %32, ptr noundef @.str.5)
  %34 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8) %33, ptr noundef @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_)
  ret void
}

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #5

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_c(ptr noundef nonnull align 8 dereferenceable(8), i8 noundef signext) #5

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8), i32 noundef) #5

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEPFRSoS_E(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef) #5

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_(ptr noundef nonnull align 8 dereferenceable(8)) #5

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) unnamed_addr #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 0
  store ptr null, ptr %4, align 8
  %5 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 1
  store i32 0, ptr %5, align 8
  %6 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 2
  store i32 0, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) unnamed_addr #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %7, align 8
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %8, align 8
  store ptr %0, ptr %5, align 8
  %9 = load ptr, ptr %5, align 8
  %10 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  store ptr null, ptr %10, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %4, i64 16, i1 false)
  %11 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  %12 = load i64, ptr %11, align 8
  %13 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  %14 = load ptr, ptr %13, align 8
  call void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %9, i64 %12, ptr %14)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2, i32 noundef %3) unnamed_addr #0 align 2 {
  %5 = alloca %"class.std::basic_string_view", align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca %"class.std::basic_string_view", align 8
  %9 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  store i64 %1, ptr %9, align 8
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  store ptr %2, ptr %10, align 8
  store ptr %0, ptr %6, align 8
  store i32 %3, ptr %7, align 4
  %11 = load ptr, ptr %6, align 8
  %12 = getelementptr inbounds nuw %class.MyString, ptr %11, i32 0, i32 0
  store ptr null, ptr %12, align 8
  %13 = load i32, ptr %7, align 4
  %14 = sext i32 %13 to i64
  %15 = call { i64, ptr } @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE6substrEmm(ptr noundef nonnull align 8 dereferenceable(16) %5, i64 noundef 0, i64 noundef %14)
  %16 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 0
  %17 = extractvalue { i64, ptr } %15, 0
  store i64 %17, ptr %16, align 8
  %18 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 1
  %19 = extractvalue { i64, ptr } %15, 1
  store ptr %19, ptr %18, align 8
  %20 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 0
  %21 = load i64, ptr %20, align 8
  %22 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 1
  %23 = load ptr, ptr %22, align 8
  call void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %11, i64 %21, ptr %23)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define linkonce_odr dso_local { i64, ptr } @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE6substrEmm(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 noundef %1, i64 noundef %2) #0 comdat align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  %8 = alloca i64, align 8
  %9 = alloca i64, align 8
  store ptr %0, ptr %5, align 8
  store i64 %1, ptr %6, align 8
  store i64 %2, ptr %7, align 8
  %10 = load ptr, ptr %5, align 8
  %11 = call noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %10) #12
  %12 = load i64, ptr %6, align 8
  %13 = call noundef i64 @_ZSt10__sv_checkmmPKc(i64 noundef %11, i64 noundef %12, ptr noundef @.str.7)
  store i64 %13, ptr %6, align 8
  %14 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %10, i32 0, i32 0
  %15 = load i64, ptr %14, align 8
  %16 = load i64, ptr %6, align 8
  %17 = sub i64 %15, %16
  store i64 %17, ptr %9, align 8
  %18 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3minImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8) %7, ptr noundef nonnull align 8 dereferenceable(8) %9)
  %19 = load i64, ptr %18, align 8
  store i64 %19, ptr %8, align 8
  %20 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %10, i32 0, i32 1
  %21 = load ptr, ptr %20, align 8
  %22 = load i64, ptr %6, align 8
  %23 = getelementptr inbounds nuw i8, ptr %21, i64 %22
  %24 = load i64, ptr %8, align 8
  call void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKcm(ptr noundef nonnull align 8 dereferenceable(16) %4, ptr noundef %23, i64 noundef %24) #12
  %25 = load { i64, ptr }, ptr %4, align 8
  ret { i64, ptr } %25
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2Eic(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i8 noundef signext %2) unnamed_addr #0 align 2 {
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i8, align 1
  store ptr %0, ptr %4, align 8
  store i32 %1, ptr %5, align 4
  store i8 %2, ptr %6, align 1
  %7 = load ptr, ptr %4, align 8
  %8 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  store ptr null, ptr %8, align 8
  %9 = load i32, ptr %5, align 4
  %10 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  store i32 %9, ptr %10, align 8
  %11 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %12 = load i32, ptr %11, align 8
  %13 = add nsw i32 %12, 1
  %14 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  store i32 %13, ptr %14, align 4
  %15 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  %16 = load i32, ptr %15, align 4
  %17 = sext i32 %16 to i64
  %18 = call noalias noundef nonnull ptr @_Znam(i64 noundef %17) #13
  %19 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  store ptr %18, ptr %19, align 8
  %20 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %21 = load ptr, ptr %20, align 8
  %22 = load i8, ptr %6, align 1
  %23 = sext i8 %22 to i32
  %24 = trunc i32 %23 to i8
  %25 = load i32, ptr %5, align 4
  %26 = sext i32 %25 to i64
  call void @llvm.memset.p0.i64(ptr align 1 %21, i8 %24, i64 %26, i1 false)
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #7

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2ERKS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) unnamed_addr #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = getelementptr inbounds nuw %class.MyString, ptr %6, i32 0, i32 0
  store ptr null, ptr %7, align 8
  %8 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %9 = call { i64, ptr } @_ZNK8MyStringcvSt17basic_string_viewIcSt11char_traitsIcEEEv(ptr noundef nonnull align 8 dereferenceable(16) %8)
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %11 = extractvalue { i64, ptr } %9, 0
  store i64 %11, ptr %10, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %13 = extractvalue { i64, ptr } %9, 1
  store ptr %13, ptr %12, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %15 = load i64, ptr %14, align 8
  %16 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %17 = load ptr, ptr %16, align 8
  call void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %15, ptr %17)
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local { i64, ptr } @_ZNK8MyStringcvSt17basic_string_viewIcSt11char_traitsIcEEEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca %"class.std::basic_string_view", align 8
  %3 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  %4 = load ptr, ptr %3, align 8
  %5 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 0
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %8 = load i32, ptr %7, align 8
  %9 = sext i32 %8 to i64
  call void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKcm(ptr noundef nonnull align 8 dereferenceable(16) %2, ptr noundef %6, i64 noundef %9) #12
  %10 = load { i64, ptr }, ptr %2, align 8
  ret { i64, ptr } %10
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2EPKc(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef %1) unnamed_addr #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = load ptr, ptr %4, align 8
  call void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKc(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef %7) #12
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %9 = load i64, ptr %8, align 8
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %11 = load ptr, ptr %10, align 8
  call void @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %9, ptr %11)
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKc(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull %1) unnamed_addr #1 comdat align 2 personality ptr @__gxx_personality_v0 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %5, i32 0, i32 0
  %7 = load ptr, ptr %4, align 8
  %8 = invoke noundef i64 @_ZNSt11char_traitsIcE6lengthEPKc(ptr noundef %7)
          to label %9 unwind label %12

9:                                                ; preds = %2
  store i64 %8, ptr %6, align 8
  %10 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %5, i32 0, i32 1
  %11 = load ptr, ptr %4, align 8
  store ptr %11, ptr %10, align 8
  ret void

12:                                               ; preds = %2
  %13 = landingpad { ptr, i32 }
          catch ptr null
  %14 = extractvalue { ptr, i32 } %13, 0
  call void @__clang_call_terminate(ptr %14) #16
  unreachable
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2ENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef %1) unnamed_addr #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = call { i64, ptr } @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEcvSt17basic_string_viewIcS2_EEv(ptr noundef nonnull align 8 dereferenceable(32) %1) #12
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %9 = extractvalue { i64, ptr } %7, 0
  store i64 %9, ptr %8, align 8
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %11 = extractvalue { i64, ptr } %7, 1
  store ptr %11, ptr %10, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %13 = load i64, ptr %12, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %15 = load ptr, ptr %14, align 8
  call void @_ZN8MyStringC2ESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %13, ptr %15)
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local { i64, ptr } @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEcvSt17basic_string_viewIcS2_EEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 {
  %2 = alloca %"class.std::basic_string_view", align 8
  %3 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  %4 = load ptr, ptr %3, align 8
  %5 = call noundef ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(32) %4) #12
  %6 = call noundef i64 @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(32) %4) #12
  call void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKcm(ptr noundef nonnull align 8 dereferenceable(16) %2, ptr noundef %5, i64 noundef %6) #12
  %7 = load { i64, ptr }, ptr %2, align 8
  ret { i64, ptr } %7
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2EOS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) unnamed_addr #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 0
  %7 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %8 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %9 = load ptr, ptr %8, align 8
  store ptr %9, ptr %6, align 8
  %10 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 1
  %11 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %12 = getelementptr inbounds nuw %class.MyString, ptr %11, i32 0, i32 1
  %13 = load i32, ptr %12, align 8
  store i32 %13, ptr %10, align 8
  %14 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 2
  %15 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %16 = getelementptr inbounds nuw %class.MyString, ptr %15, i32 0, i32 2
  %17 = load i32, ptr %16, align 4
  store i32 %17, ptr %14, align 4
  %18 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %19 = getelementptr inbounds nuw %class.MyString, ptr %18, i32 0, i32 0
  store ptr null, ptr %19, align 8
  %20 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %21 = getelementptr inbounds nuw %class.MyString, ptr %20, i32 0, i32 1
  store i32 0, ptr %21, align 8
  %22 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %23 = getelementptr inbounds nuw %class.MyString, ptr %22, i32 0, i32 2
  store i32 0, ptr %23, align 4
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2Ei(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1) unnamed_addr #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca i8, align 1
  %6 = alloca i32, align 4
  %7 = alloca i8, align 1
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %8 = load ptr, ptr %3, align 8
  %9 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  store ptr null, ptr %9, align 8
  %10 = load i32, ptr %4, align 4
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %12, label %13

12:                                               ; preds = %2
  call void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %8, i32 noundef 0, i32 noundef 1, i8 noundef signext 48)
  br label %13

13:                                               ; preds = %12, %2
  store i8 0, ptr %5, align 1
  %14 = load i32, ptr %4, align 4
  store i32 %14, ptr %6, align 4
  %15 = load i32, ptr %6, align 4
  %16 = icmp slt i32 %15, 0
  br i1 %16, label %17, label %20

17:                                               ; preds = %13
  store i8 1, ptr %5, align 1
  %18 = load i32, ptr %6, align 4
  %19 = sub nsw i32 0, %18
  store i32 %19, ptr %6, align 4
  br label %20

20:                                               ; preds = %17, %13
  br label %21

21:                                               ; preds = %24, %20
  %22 = load i32, ptr %6, align 4
  %23 = icmp sgt i32 %22, 0
  br i1 %23, label %24, label %32

24:                                               ; preds = %21
  %25 = load i32, ptr %6, align 4
  %26 = srem i32 %25, 10
  %27 = add nsw i32 48, %26
  %28 = trunc i32 %27 to i8
  store i8 %28, ptr %7, align 1
  %29 = load i8, ptr %7, align 1
  call void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %8, i32 noundef 0, i32 noundef 1, i8 noundef signext %29)
  %30 = load i32, ptr %6, align 4
  %31 = sdiv i32 %30, 10
  store i32 %31, ptr %6, align 4
  br label %21, !llvm.loop !10

32:                                               ; preds = %21
  %33 = load i8, ptr %5, align 1
  %34 = trunc i8 %33 to i1
  br i1 %34, label %35, label %36

35:                                               ; preds = %32
  call void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %8, i32 noundef 0, i32 noundef 1, i8 noundef signext 45)
  br label %36

36:                                               ; preds = %35, %32
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2, i8 noundef signext %3) #0 align 2 {
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i8, align 1
  %9 = alloca ptr, align 8
  store ptr %0, ptr %5, align 8
  store i32 %1, ptr %6, align 4
  store i32 %2, ptr %7, align 4
  store i8 %3, ptr %8, align 1
  %10 = load ptr, ptr %5, align 8
  %11 = load i32, ptr %7, align 4
  %12 = sext i32 %11 to i64
  %13 = call noalias noundef nonnull ptr @_Znam(i64 noundef %12) #13
  store ptr %13, ptr %9, align 8
  %14 = load ptr, ptr %9, align 8
  %15 = load i8, ptr %8, align 1
  %16 = sext i8 %15 to i32
  %17 = trunc i32 %16 to i8
  %18 = load i32, ptr %7, align 4
  %19 = sext i32 %18 to i64
  call void @llvm.memset.p0.i64(ptr align 1 %14, i8 %17, i64 %19, i1 false)
  %20 = load i32, ptr %6, align 4
  %21 = load i32, ptr %7, align 4
  %22 = load ptr, ptr %9, align 8
  call void @_ZN8MyString9my_insertEiiPKc(ptr noundef nonnull align 8 dereferenceable(16) %10, i32 noundef %20, i32 noundef %21, ptr noundef %22)
  %23 = load ptr, ptr %9, align 8
  %24 = icmp eq ptr %23, null
  br i1 %24, label %26, label %25

25:                                               ; preds = %4
  call void @_ZdaPv(ptr noundef %23) #14
  br label %26

26:                                               ; preds = %25, %4
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringC2Ef(ptr noundef nonnull align 8 dereferenceable(16) %0, float noundef %1) unnamed_addr #0 align 2 personality ptr @__gxx_personality_v0 {
  %3 = alloca ptr, align 8
  %4 = alloca float, align 4
  %5 = alloca i32, align 4
  %6 = alloca %class.MyString, align 8
  %7 = alloca i32, align 4
  %8 = alloca ptr, align 8
  %9 = alloca i32, align 4
  %10 = alloca float, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i8, align 1
  store ptr %0, ptr %3, align 8
  store float %1, ptr %4, align 4
  %14 = load ptr, ptr %3, align 8
  %15 = getelementptr inbounds nuw %class.MyString, ptr %14, i32 0, i32 0
  store ptr null, ptr %15, align 8
  %16 = load float, ptr %4, align 4
  %17 = fcmp olt float %16, 0.000000e+00
  br i1 %17, label %18, label %19

18:                                               ; preds = %2
  call void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %14, i32 noundef 0, i32 noundef 1, i8 noundef signext 45)
  br label %19

19:                                               ; preds = %18, %2
  %20 = load float, ptr %4, align 4
  %21 = fptosi float %20 to i32
  store i32 %21, ptr %5, align 4
  %22 = load i32, ptr %5, align 4
  call void @_ZN8MyStringC1Ei(ptr noundef nonnull align 8 dereferenceable(16) %6, i32 noundef %22)
  store i32 0, ptr %7, align 4
  br label %23

23:                                               ; preds = %36, %19
  %24 = load i32, ptr %7, align 4
  %25 = invoke noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %6)
          to label %26 unwind label %39

26:                                               ; preds = %23
  %27 = icmp slt i32 %24, %25
  br i1 %27, label %28, label %43

28:                                               ; preds = %26
  %29 = invoke noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %14)
          to label %30 unwind label %39

30:                                               ; preds = %28
  %31 = load i32, ptr %7, align 4
  %32 = invoke noundef nonnull align 1 dereferenceable(1) ptr @_ZN8MyStringixEi(ptr noundef nonnull align 8 dereferenceable(16) %6, i32 noundef %31)
          to label %33 unwind label %39

33:                                               ; preds = %30
  %34 = load i8, ptr %32, align 1
  invoke void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %14, i32 noundef %29, i32 noundef 1, i8 noundef signext %34)
          to label %35 unwind label %39

35:                                               ; preds = %33
  br label %36

36:                                               ; preds = %35
  %37 = load i32, ptr %7, align 4
  %38 = add nsw i32 %37, 1
  store i32 %38, ptr %7, align 4
  br label %23, !llvm.loop !11

39:                                               ; preds = %66, %57, %52, %50, %33, %30, %28, %23
  %40 = landingpad { ptr, i32 }
          cleanup
  %41 = extractvalue { ptr, i32 } %40, 0
  store ptr %41, ptr %8, align 8
  %42 = extractvalue { ptr, i32 } %40, 1
  store i32 %42, ptr %9, align 4
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  br label %81

43:                                               ; preds = %26
  %44 = load float, ptr %4, align 4
  %45 = load i32, ptr %5, align 4
  %46 = sitofp i32 %45 to float
  %47 = fsub float %44, %46
  store float %47, ptr %10, align 4
  %48 = load float, ptr %10, align 4
  %49 = fcmp ogt float %48, 0x3EE4F8B580000000
  br i1 %49, label %50, label %80

50:                                               ; preds = %43
  %51 = invoke noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %14)
          to label %52 unwind label %39

52:                                               ; preds = %50
  invoke void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %14, i32 noundef %51, i32 noundef 1, i8 noundef signext 46)
          to label %53 unwind label %39

53:                                               ; preds = %52
  store i32 0, ptr %11, align 4
  br label %54

54:                                               ; preds = %78, %53
  %55 = load i32, ptr %11, align 4
  %56 = icmp slt i32 %55, 6
  br i1 %56, label %57, label %79

57:                                               ; preds = %54
  %58 = load float, ptr %10, align 4
  %59 = fmul float %58, 1.000000e+01
  store float %59, ptr %10, align 4
  %60 = load float, ptr %10, align 4
  %61 = fptosi float %60 to i32
  store i32 %61, ptr %12, align 4
  %62 = load i32, ptr %12, align 4
  %63 = add nsw i32 48, %62
  %64 = trunc i32 %63 to i8
  store i8 %64, ptr %13, align 1
  %65 = invoke noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %14)
          to label %66 unwind label %39

66:                                               ; preds = %57
  %67 = load i8, ptr %13, align 1
  invoke void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %14, i32 noundef %65, i32 noundef 1, i8 noundef signext %67)
          to label %68 unwind label %39

68:                                               ; preds = %66
  %69 = load i32, ptr %12, align 4
  %70 = sitofp i32 %69 to float
  %71 = load float, ptr %10, align 4
  %72 = fsub float %71, %70
  store float %72, ptr %10, align 4
  %73 = load i32, ptr %11, align 4
  %74 = add nsw i32 %73, 1
  store i32 %74, ptr %11, align 4
  %75 = load float, ptr %10, align 4
  %76 = fcmp olt float %75, 0x3EE4F8B580000000
  br i1 %76, label %77, label %78

77:                                               ; preds = %68
  br label %79

78:                                               ; preds = %68
  br label %54, !llvm.loop !12

79:                                               ; preds = %77, %54
  br label %80

80:                                               ; preds = %79, %43
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  ret void

81:                                               ; preds = %39
  %82 = load ptr, ptr %8, align 8
  %83 = load i32, ptr %9, align 4
  %84 = insertvalue { ptr, i32 } poison, ptr %82, 0
  %85 = insertvalue { ptr, i32 } %84, i32 %83, 1
  resume { ptr, i32 } %85
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 1
  %5 = load i32, ptr %4, align 8
  ret i32 %5
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef nonnull align 1 dereferenceable(1) ptr @_ZN8MyStringixEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1) #0 align 2 personality ptr @__gxx_personality_v0 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %7 = load ptr, ptr %3, align 8
  %8 = load i32, ptr %4, align 4
  %9 = icmp slt i32 %8, 0
  br i1 %9, label %10, label %15

10:                                               ; preds = %2
  %11 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  %12 = load i32, ptr %11, align 4
  %13 = load i32, ptr %4, align 4
  %14 = sub nsw i32 %12, %13
  store i32 %14, ptr %4, align 4
  br label %15

15:                                               ; preds = %10, %2
  %16 = load i32, ptr %4, align 4
  %17 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  %18 = load i32, ptr %17, align 4
  %19 = icmp sgt i32 %16, %18
  br i1 %19, label %23, label %20

20:                                               ; preds = %15
  %21 = load i32, ptr %4, align 4
  %22 = icmp slt i32 %21, 0
  br i1 %22, label %23, label %30

23:                                               ; preds = %20, %15
  %24 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %24, ptr noundef @.str)
          to label %25 unwind label %26

25:                                               ; preds = %23
  call void @__cxa_throw(ptr %24, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

26:                                               ; preds = %23
  %27 = landingpad { ptr, i32 }
          cleanup
  %28 = extractvalue { ptr, i32 } %27, 0
  store ptr %28, ptr %5, align 8
  %29 = extractvalue { ptr, i32 } %27, 1
  store i32 %29, ptr %6, align 4
  call void @__cxa_free_exception(ptr %24) #12
  br label %36

30:                                               ; preds = %20
  %31 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %32 = load ptr, ptr %31, align 8
  %33 = load i32, ptr %4, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i8, ptr %32, i64 %34
  ret ptr %35

36:                                               ; preds = %26
  %37 = load ptr, ptr %5, align 8
  %38 = load i32, ptr %6, align 4
  %39 = insertvalue { ptr, i32 } poison, ptr %37, 0
  %40 = insertvalue { ptr, i32 } %39, i32 %38, 1
  resume { ptr, i32 } %40
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local void @_ZN8MyStringD2Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) unnamed_addr #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  %6 = icmp eq ptr %5, null
  br i1 %6, label %8, label %7

7:                                                ; preds = %1
  call void @_ZdaPv(ptr noundef %5) #14
  br label %8

8:                                                ; preds = %7, %1
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local void @_ZN8MyString5clearEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  %6 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 2
  %7 = load i32, ptr %6, align 4
  %8 = sext i32 %7 to i64
  call void @llvm.memset.p0.i64(ptr align 1 %5, i8 0, i64 %8, i1 false)
  %9 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 1
  store i32 0, ptr %9, align 8
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString13shrink_to_fitEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #0 align 2 {
  %2 = alloca ptr, align 8
  %3 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %4 = load ptr, ptr %2, align 8
  %5 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %6 = load i32, ptr %5, align 8
  %7 = add nsw i32 %6, 1
  %8 = call noundef ptr @_ZN8MyString19create_copy_of_pstrEi(ptr noundef nonnull align 8 dereferenceable(16) %4, i32 noundef %7)
  store ptr %8, ptr %3, align 8
  %9 = load ptr, ptr %3, align 8
  %10 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %11 = load i32, ptr %10, align 8
  %12 = getelementptr inbounds nuw %class.MyString, ptr %4, i32 0, i32 1
  %13 = load i32, ptr %12, align 8
  %14 = add nsw i32 %13, 1
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %4, ptr noundef %9, i32 noundef %11, i32 noundef %14)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringaSESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %7, align 8
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %8, align 8
  store ptr %0, ptr %5, align 8
  %9 = load ptr, ptr %5, align 8
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %9, ptr noundef null, i32 noundef 0, i32 noundef 0)
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %4, i64 16, i1 false)
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  %11 = load i64, ptr %10, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  %13 = load ptr, ptr %12, align 8
  call void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %9, i64 %11, ptr %13)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringaSEPKc(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef %1) #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = load ptr, ptr %4, align 8
  call void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKc(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef %7) #12
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %9 = load i64, ptr %8, align 8
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %11 = load ptr, ptr %10, align 8
  call void @_ZN8MyStringaSESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %9, ptr %11)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringaSERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(32) %1) #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %8 = call { i64, ptr } @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEcvSt17basic_string_viewIcS2_EEv(ptr noundef nonnull align 8 dereferenceable(32) %7) #12
  %9 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %10 = extractvalue { i64, ptr } %8, 0
  store i64 %10, ptr %9, align 8
  %11 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %12 = extractvalue { i64, ptr } %8, 1
  store ptr %12, ptr %11, align 8
  %13 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %14 = load i64, ptr %13, align 8
  %15 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %16 = load ptr, ptr %15, align 8
  call void @_ZN8MyStringaSESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %14, ptr %16)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyStringaSEc(ptr noundef nonnull align 8 dereferenceable(16) %0, i8 noundef signext %1) #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca i8, align 1
  store ptr %0, ptr %3, align 8
  store i8 %1, ptr %4, align 1
  %5 = load ptr, ptr %3, align 8
  %6 = call noalias noundef nonnull ptr @_Znam(i64 noundef 2) #13
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef %6, i32 noundef 1, i32 noundef 2)
  %7 = load i8, ptr %4, align 1
  %8 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 0
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr inbounds i8, ptr %9, i64 0
  store i8 %7, ptr %10, align 1
  %11 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 0
  %12 = load ptr, ptr %11, align 8
  %13 = getelementptr inbounds i8, ptr %12, i64 1
  store i8 0, ptr %13, align 1
  ret void
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef nonnull align 8 dereferenceable(16) ptr @_ZN8MyStringaSEOS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = icmp ne ptr %5, %6
  br i1 %7, label %8, label %27

8:                                                ; preds = %2
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef null, i32 noundef 0, i32 noundef 0)
  %9 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %10 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  %11 = load ptr, ptr %10, align 8
  %12 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 0
  store ptr %11, ptr %12, align 8
  %13 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %14 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %15 = load i32, ptr %14, align 8
  %16 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 1
  store i32 %15, ptr %16, align 8
  %17 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %18 = getelementptr inbounds nuw %class.MyString, ptr %17, i32 0, i32 2
  %19 = load i32, ptr %18, align 4
  %20 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 2
  store i32 %19, ptr %20, align 4
  %21 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %22 = getelementptr inbounds nuw %class.MyString, ptr %21, i32 0, i32 0
  store ptr null, ptr %22, align 8
  %23 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %24 = getelementptr inbounds nuw %class.MyString, ptr %23, i32 0, i32 1
  store i32 0, ptr %24, align 8
  %25 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %26 = getelementptr inbounds nuw %class.MyString, ptr %25, i32 0, i32 2
  store i32 0, ptr %26, align 4
  br label %27

27:                                               ; preds = %8, %2
  ret ptr %5
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef nonnull align 8 dereferenceable(16) ptr @_ZN8MyStringaSERKS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #0 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca %"class.std::basic_string_view", align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %6 = load ptr, ptr %3, align 8
  %7 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %8 = icmp ne ptr %6, %7
  br i1 %8, label %9, label %20

9:                                                ; preds = %2
  call void @_ZN8MyString25delete_pstr_change_paramsEPcii(ptr noundef nonnull align 8 dereferenceable(16) %6, ptr noundef null, i32 noundef 0, i32 noundef 0)
  %10 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %11 = call { i64, ptr } @_ZNK8MyStringcvSt17basic_string_viewIcSt11char_traitsIcEEEv(ptr noundef nonnull align 8 dereferenceable(16) %10)
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %13 = extractvalue { i64, ptr } %11, 0
  store i64 %13, ptr %12, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %15 = extractvalue { i64, ptr } %11, 1
  store ptr %15, ptr %14, align 8
  %16 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  %17 = load i64, ptr %16, align 8
  %18 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  %19 = load ptr, ptr %18, align 8
  call void @_ZN8MyString4initESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %6, i64 %17, ptr %19)
  br label %20

20:                                               ; preds = %9, %2
  ret ptr %6
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef ptr @_ZNK8MyString5c_strEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  ret ptr %5
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef i32 @_ZNK8MyString8capacityEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 2
  %5 = load i32, ptr %4, align 4
  ret i32 %5
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyString5emptyEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %class.MyString, ptr %3, i32 0, i32 1
  %5 = load i32, ptr %4, align 8
  %6 = icmp eq i32 %5, 0
  ret i1 %6
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i64 %2, ptr %3) #0 align 2 {
  %5 = alloca %"class.std::basic_string_view", align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  store i64 %2, ptr %8, align 8
  %9 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  store ptr %3, ptr %9, align 8
  store ptr %0, ptr %6, align 8
  store i32 %1, ptr %7, align 4
  %10 = load ptr, ptr %6, align 8
  %11 = load i32, ptr %7, align 4
  %12 = call noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %5) #12
  %13 = trunc i64 %12 to i32
  %14 = call noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %5) #12
  call void @_ZN8MyString9my_insertEiiPKc(ptr noundef nonnull align 8 dereferenceable(16) %10, i32 noundef %11, i32 noundef %13, ptr noundef %14)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i64 %2, ptr %3, i32 noundef %4) #0 align 2 {
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = alloca ptr, align 8
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  store i64 %2, ptr %10, align 8
  %11 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  store ptr %3, ptr %11, align 8
  store ptr %0, ptr %7, align 8
  store i32 %1, ptr %8, align 4
  store i32 %4, ptr %9, align 4
  %12 = load ptr, ptr %7, align 8
  %13 = load i32, ptr %8, align 4
  %14 = load i32, ptr %9, align 4
  %15 = call noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  call void @_ZN8MyString9my_insertEiiPKc(ptr noundef nonnull align 8 dereferenceable(16) %12, i32 noundef %13, i32 noundef %14, ptr noundef %15)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i64 %2, ptr %3, i32 noundef %4, i32 noundef %5) #0 align 2 {
  %7 = alloca %"class.std::basic_string_view", align 8
  %8 = alloca ptr, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca %"class.std::basic_string_view", align 8
  %13 = getelementptr inbounds nuw { i64, ptr }, ptr %7, i32 0, i32 0
  store i64 %2, ptr %13, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %7, i32 0, i32 1
  store ptr %3, ptr %14, align 8
  store ptr %0, ptr %8, align 8
  store i32 %1, ptr %9, align 4
  store i32 %4, ptr %10, align 4
  store i32 %5, ptr %11, align 4
  %15 = load ptr, ptr %8, align 8
  %16 = load i32, ptr %10, align 4
  %17 = sext i32 %16 to i64
  %18 = load i32, ptr %11, align 4
  %19 = sext i32 %18 to i64
  %20 = call { i64, ptr } @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE6substrEmm(ptr noundef nonnull align 8 dereferenceable(16) %7, i64 noundef %17, i64 noundef %19)
  %21 = getelementptr inbounds nuw { i64, ptr }, ptr %12, i32 0, i32 0
  %22 = extractvalue { i64, ptr } %20, 0
  store i64 %22, ptr %21, align 8
  %23 = getelementptr inbounds nuw { i64, ptr }, ptr %12, i32 0, i32 1
  %24 = extractvalue { i64, ptr } %20, 1
  store ptr %24, ptr %23, align 8
  %25 = load i32, ptr %9, align 4
  %26 = load i32, ptr %11, align 4
  %27 = call noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %12) #12
  call void @_ZN8MyString9my_insertEiiPKc(ptr noundef nonnull align 8 dereferenceable(16) %15, i32 noundef %25, i32 noundef %26, ptr noundef %27)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6appendEic(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i8 noundef signext %2) #0 align 2 {
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i8, align 1
  store ptr %0, ptr %4, align 8
  store i32 %1, ptr %5, align 4
  store i8 %2, ptr %6, align 1
  %7 = load ptr, ptr %4, align 8
  %8 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %9 = load i32, ptr %8, align 8
  %10 = load i32, ptr %5, align 4
  %11 = load i8, ptr %6, align 1
  call void @_ZN8MyString6insertEiic(ptr noundef nonnull align 8 dereferenceable(16) %7, i32 noundef %9, i32 noundef %10, i8 noundef signext %11)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6appendESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %7, align 8
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %8, align 8
  store ptr %0, ptr %5, align 8
  %9 = load ptr, ptr %5, align 8
  %10 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 1
  %11 = load i32, ptr %10, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %4, i64 16, i1 false)
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  %13 = load i64, ptr %12, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  %15 = load ptr, ptr %14, align 8
  call void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %9, i32 noundef %11, i64 %13, ptr %15)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6appendESt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2, i32 noundef %3) #0 align 2 {
  %5 = alloca %"class.std::basic_string_view", align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca %"class.std::basic_string_view", align 8
  %9 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 0
  store i64 %1, ptr %9, align 8
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %5, i32 0, i32 1
  store ptr %2, ptr %10, align 8
  store ptr %0, ptr %6, align 8
  store i32 %3, ptr %7, align 4
  %11 = load ptr, ptr %6, align 8
  %12 = getelementptr inbounds nuw %class.MyString, ptr %11, i32 0, i32 1
  %13 = load i32, ptr %12, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %8, ptr align 8 %5, i64 16, i1 false)
  %14 = load i32, ptr %7, align 4
  %15 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 0
  %16 = load i64, ptr %15, align 8
  %17 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 1
  %18 = load ptr, ptr %17, align 8
  call void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %11, i32 noundef %13, i64 %16, ptr %18, i32 noundef %14)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString6appendESt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2, i32 noundef %3, i32 noundef %4) #0 align 2 {
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = alloca ptr, align 8
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca %"class.std::basic_string_view", align 8
  %11 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  store i64 %1, ptr %11, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  store ptr %2, ptr %12, align 8
  store ptr %0, ptr %7, align 8
  store i32 %3, ptr %8, align 4
  store i32 %4, ptr %9, align 4
  %13 = load ptr, ptr %7, align 8
  %14 = getelementptr inbounds nuw %class.MyString, ptr %13, i32 0, i32 1
  %15 = load i32, ptr %14, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %10, ptr align 8 %6, i64 16, i1 false)
  %16 = load i32, ptr %8, align 4
  %17 = load i32, ptr %9, align 4
  %18 = getelementptr inbounds nuw { i64, ptr }, ptr %10, i32 0, i32 0
  %19 = load i64, ptr %18, align 8
  %20 = getelementptr inbounds nuw { i64, ptr }, ptr %10, i32 0, i32 1
  %21 = load ptr, ptr %20, align 8
  call void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %13, i32 noundef %15, i64 %19, ptr %21, i32 noundef %16, i32 noundef %17)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString5eraseEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2) #0 align 2 personality ptr @__gxx_personality_v0 {
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca ptr, align 8
  %8 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store i32 %1, ptr %5, align 4
  store i32 %2, ptr %6, align 4
  %9 = load ptr, ptr %4, align 8
  %10 = load i32, ptr %5, align 4
  %11 = icmp slt i32 %10, 0
  br i1 %11, label %12, label %17

12:                                               ; preds = %3
  %13 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 2
  %14 = load i32, ptr %13, align 4
  %15 = load i32, ptr %5, align 4
  %16 = sub nsw i32 %14, %15
  store i32 %16, ptr %5, align 4
  br label %17

17:                                               ; preds = %12, %3
  %18 = load i32, ptr %5, align 4
  %19 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 2
  %20 = load i32, ptr %19, align 4
  %21 = icmp sgt i32 %18, %20
  br i1 %21, label %25, label %22

22:                                               ; preds = %17
  %23 = load i32, ptr %5, align 4
  %24 = icmp slt i32 %23, 0
  br i1 %24, label %25, label %32

25:                                               ; preds = %22, %17
  %26 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %26, ptr noundef @.str)
          to label %27 unwind label %28

27:                                               ; preds = %25
  call void @__cxa_throw(ptr %26, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

28:                                               ; preds = %25
  %29 = landingpad { ptr, i32 }
          cleanup
  %30 = extractvalue { ptr, i32 } %29, 0
  store ptr %30, ptr %7, align 8
  %31 = extractvalue { ptr, i32 } %29, 1
  store i32 %31, ptr %8, align 4
  call void @__cxa_free_exception(ptr %26) #12
  br label %65

32:                                               ; preds = %22
  %33 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  %34 = load ptr, ptr %33, align 8
  %35 = load i32, ptr %5, align 4
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds i8, ptr %34, i64 %36
  %38 = load i32, ptr %6, align 4
  %39 = sext i32 %38 to i64
  call void @llvm.memset.p0.i64(ptr align 1 %37, i8 0, i64 %39, i1 false)
  %40 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  %41 = load ptr, ptr %40, align 8
  %42 = load i32, ptr %5, align 4
  %43 = sext i32 %42 to i64
  %44 = getelementptr inbounds i8, ptr %41, i64 %43
  %45 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 0
  %46 = load ptr, ptr %45, align 8
  %47 = load i32, ptr %5, align 4
  %48 = sext i32 %47 to i64
  %49 = getelementptr inbounds i8, ptr %46, i64 %48
  %50 = load i32, ptr %6, align 4
  %51 = sext i32 %50 to i64
  %52 = getelementptr inbounds i8, ptr %49, i64 %51
  %53 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 2
  %54 = load i32, ptr %53, align 4
  %55 = load i32, ptr %5, align 4
  %56 = sub nsw i32 %54, %55
  %57 = load i32, ptr %6, align 4
  %58 = sub nsw i32 %56, %57
  %59 = sext i32 %58 to i64
  call void @llvm.memmove.p0.p0.i64(ptr align 1 %44, ptr align 1 %52, i64 %59, i1 false)
  %60 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 1
  %61 = load i32, ptr %60, align 8
  %62 = load i32, ptr %6, align 4
  %63 = sub nsw i32 %61, %62
  %64 = getelementptr inbounds nuw %class.MyString, ptr %9, i32 0, i32 1
  store i32 %63, ptr %64, align 8
  ret void

65:                                               ; preds = %28
  %66 = load ptr, ptr %7, align 8
  %67 = load i32, ptr %8, align 4
  %68 = insertvalue { ptr, i32 } poison, ptr %66, 0
  %69 = insertvalue { ptr, i32 } %68, i32 %67, 1
  resume { ptr, i32 } %69
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString7replaceEiiSt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2, i64 %3, ptr %4, i32 noundef %5, i32 noundef %6) #0 align 2 {
  %8 = alloca %"class.std::basic_string_view", align 8
  %9 = alloca ptr, align 8
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca %"class.std::basic_string_view", align 8
  %15 = alloca %"class.std::basic_string_view", align 8
  %16 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 0
  store i64 %3, ptr %16, align 8
  %17 = getelementptr inbounds nuw { i64, ptr }, ptr %8, i32 0, i32 1
  store ptr %4, ptr %17, align 8
  store ptr %0, ptr %9, align 8
  store i32 %1, ptr %10, align 4
  store i32 %2, ptr %11, align 4
  store i32 %5, ptr %12, align 4
  store i32 %6, ptr %13, align 4
  %18 = load ptr, ptr %9, align 8
  %19 = load i32, ptr %12, align 4
  %20 = sext i32 %19 to i64
  %21 = load i32, ptr %13, align 4
  %22 = sext i32 %21 to i64
  %23 = call { i64, ptr } @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE6substrEmm(ptr noundef nonnull align 8 dereferenceable(16) %8, i64 noundef %20, i64 noundef %22)
  %24 = getelementptr inbounds nuw { i64, ptr }, ptr %14, i32 0, i32 0
  %25 = extractvalue { i64, ptr } %23, 0
  store i64 %25, ptr %24, align 8
  %26 = getelementptr inbounds nuw { i64, ptr }, ptr %14, i32 0, i32 1
  %27 = extractvalue { i64, ptr } %23, 1
  store ptr %27, ptr %26, align 8
  %28 = load i32, ptr %10, align 4
  %29 = load i32, ptr %11, align 4
  call void @_ZN8MyString5eraseEii(ptr noundef nonnull align 8 dereferenceable(16) %18, i32 noundef %28, i32 noundef %29)
  %30 = load i32, ptr %10, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %15, ptr align 8 %14, i64 16, i1 false)
  %31 = getelementptr inbounds nuw { i64, ptr }, ptr %15, i32 0, i32 0
  %32 = load i64, ptr %31, align 8
  %33 = getelementptr inbounds nuw { i64, ptr }, ptr %15, i32 0, i32 1
  %34 = load ptr, ptr %33, align 8
  call void @_ZN8MyString6insertEiSt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %18, i32 noundef %30, i64 %32, ptr %34)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString7replaceEiiSt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2, i64 %3, ptr %4) #0 align 2 {
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = alloca ptr, align 8
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca %"class.std::basic_string_view", align 8
  %11 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  store i64 %3, ptr %11, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  store ptr %4, ptr %12, align 8
  store ptr %0, ptr %7, align 8
  store i32 %1, ptr %8, align 4
  store i32 %2, ptr %9, align 4
  %13 = load ptr, ptr %7, align 8
  %14 = load i32, ptr %8, align 4
  %15 = load i32, ptr %9, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %10, ptr align 8 %6, i64 16, i1 false)
  %16 = call noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  %17 = trunc i64 %16 to i32
  %18 = getelementptr inbounds nuw { i64, ptr }, ptr %10, i32 0, i32 0
  %19 = load i64, ptr %18, align 8
  %20 = getelementptr inbounds nuw { i64, ptr }, ptr %10, i32 0, i32 1
  %21 = load ptr, ptr %20, align 8
  call void @_ZN8MyString7replaceEiiSt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %13, i32 noundef %14, i32 noundef %15, i64 %19, ptr %21, i32 noundef 0, i32 noundef %17)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZN8MyString7replaceEiiSt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1, i32 noundef %2, i64 %3, ptr %4, i32 noundef %5) #0 align 2 {
  %7 = alloca %"class.std::basic_string_view", align 8
  %8 = alloca ptr, align 8
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca %"class.std::basic_string_view", align 8
  %13 = getelementptr inbounds nuw { i64, ptr }, ptr %7, i32 0, i32 0
  store i64 %3, ptr %13, align 8
  %14 = getelementptr inbounds nuw { i64, ptr }, ptr %7, i32 0, i32 1
  store ptr %4, ptr %14, align 8
  store ptr %0, ptr %8, align 8
  store i32 %1, ptr %9, align 4
  store i32 %2, ptr %10, align 4
  store i32 %5, ptr %11, align 4
  %15 = load ptr, ptr %8, align 8
  %16 = load i32, ptr %9, align 4
  %17 = load i32, ptr %10, align 4
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %12, ptr align 8 %7, i64 16, i1 false)
  %18 = load i32, ptr %11, align 4
  %19 = getelementptr inbounds nuw { i64, ptr }, ptr %12, i32 0, i32 0
  %20 = load i64, ptr %19, align 8
  %21 = getelementptr inbounds nuw { i64, ptr }, ptr %12, i32 0, i32 1
  %22 = load ptr, ptr %21, align 8
  call void @_ZN8MyString7replaceEiiSt17basic_string_viewIcSt11char_traitsIcEEii(ptr noundef nonnull align 8 dereferenceable(16) %15, i32 noundef %16, i32 noundef %17, i64 %20, ptr %22, i32 noundef 0, i32 noundef %18)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZNK8MyString6substrEii(ptr dead_on_unwind noalias writable sret(%class.MyString) align 8 %0, ptr noundef nonnull align 8 dereferenceable(16) %1, i32 noundef %2, i32 noundef %3) #0 align 2 personality ptr @__gxx_personality_v0 {
  %5 = alloca ptr, align 8
  %6 = alloca ptr, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca ptr, align 8
  %10 = alloca i32, align 4
  %11 = alloca i1, align 1
  store ptr %0, ptr %5, align 8
  store ptr %1, ptr %6, align 8
  store i32 %2, ptr %7, align 4
  store i32 %3, ptr %8, align 4
  %12 = load ptr, ptr %6, align 8
  %13 = load i32, ptr %7, align 4
  %14 = icmp slt i32 %13, 0
  br i1 %14, label %15, label %20

15:                                               ; preds = %4
  %16 = getelementptr inbounds nuw %class.MyString, ptr %12, i32 0, i32 2
  %17 = load i32, ptr %16, align 4
  %18 = load i32, ptr %7, align 4
  %19 = sub nsw i32 %17, %18
  store i32 %19, ptr %7, align 4
  br label %20

20:                                               ; preds = %15, %4
  %21 = load i32, ptr %7, align 4
  %22 = getelementptr inbounds nuw %class.MyString, ptr %12, i32 0, i32 2
  %23 = load i32, ptr %22, align 4
  %24 = icmp sgt i32 %21, %23
  br i1 %24, label %28, label %25

25:                                               ; preds = %20
  %26 = load i32, ptr %7, align 4
  %27 = icmp slt i32 %26, 0
  br i1 %27, label %28, label %35

28:                                               ; preds = %25, %20
  %29 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %29, ptr noundef @.str)
          to label %30 unwind label %31

30:                                               ; preds = %28
  call void @__cxa_throw(ptr %29, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

31:                                               ; preds = %28
  %32 = landingpad { ptr, i32 }
          cleanup
  %33 = extractvalue { ptr, i32 } %32, 0
  store ptr %33, ptr %9, align 8
  %34 = extractvalue { ptr, i32 } %32, 1
  store i32 %34, ptr %10, align 4
  call void @__cxa_free_exception(ptr %29) #12
  br label %72

35:                                               ; preds = %25
  %36 = load i32, ptr %8, align 4
  %37 = load i32, ptr %7, align 4
  %38 = add nsw i32 %36, %37
  %39 = getelementptr inbounds nuw %class.MyString, ptr %12, i32 0, i32 2
  %40 = load i32, ptr %39, align 4
  %41 = icmp sgt i32 %38, %40
  br i1 %41, label %42, label %49

42:                                               ; preds = %35
  %43 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %43, ptr noundef @.str.6)
          to label %44 unwind label %45

44:                                               ; preds = %42
  call void @__cxa_throw(ptr %43, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

45:                                               ; preds = %42
  %46 = landingpad { ptr, i32 }
          cleanup
  %47 = extractvalue { ptr, i32 } %46, 0
  store ptr %47, ptr %9, align 8
  %48 = extractvalue { ptr, i32 } %46, 1
  store i32 %48, ptr %10, align 4
  call void @__cxa_free_exception(ptr %43) #12
  br label %72

49:                                               ; preds = %35
  store i1 false, ptr %11, align 1
  call void @_ZN8MyStringC1ERKS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %12)
  %50 = load i32, ptr %7, align 4
  %51 = icmp ne i32 %50, 0
  br i1 %51, label %52, label %59

52:                                               ; preds = %49
  %53 = load i32, ptr %7, align 4
  invoke void @_ZN8MyString5eraseEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef 0, i32 noundef %53)
          to label %54 unwind label %55

54:                                               ; preds = %52
  br label %59

55:                                               ; preds = %67, %59, %52
  %56 = landingpad { ptr, i32 }
          cleanup
  %57 = extractvalue { ptr, i32 } %56, 0
  store ptr %57, ptr %9, align 8
  %58 = extractvalue { ptr, i32 } %56, 1
  store i32 %58, ptr %10, align 4
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) #12
  br label %72

59:                                               ; preds = %54, %49
  %60 = load i32, ptr %8, align 4
  %61 = getelementptr inbounds nuw %class.MyString, ptr %12, i32 0, i32 1
  %62 = load i32, ptr %61, align 8
  %63 = load i32, ptr %7, align 4
  %64 = sub nsw i32 %62, %63
  %65 = load i32, ptr %8, align 4
  %66 = sub nsw i32 %64, %65
  invoke void @_ZN8MyString5eraseEii(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %60, i32 noundef %66)
          to label %67 unwind label %55

67:                                               ; preds = %59
  invoke void @_ZN8MyString13shrink_to_fitEv(ptr noundef nonnull align 8 dereferenceable(16) %0)
          to label %68 unwind label %55

68:                                               ; preds = %67
  store i1 true, ptr %11, align 1
  %69 = load i1, ptr %11, align 1
  br i1 %69, label %71, label %70

70:                                               ; preds = %68
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) #12
  br label %71

71:                                               ; preds = %70, %68
  ret void

72:                                               ; preds = %55, %45, %31
  %73 = load ptr, ptr %9, align 8
  %74 = load i32, ptr %10, align 4
  %75 = insertvalue { ptr, i32 } poison, ptr %73, 0
  %76 = insertvalue { ptr, i32 } %75, i32 %74, 1
  resume { ptr, i32 } %76
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZNK8MyString6substrEi(ptr dead_on_unwind noalias writable sret(%class.MyString) align 8 %0, ptr noundef nonnull align 8 dereferenceable(16) %1, i32 noundef %2) #0 align 2 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store i32 %2, ptr %6, align 4
  %7 = load ptr, ptr %5, align 8
  %8 = load i32, ptr %6, align 4
  %9 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %10 = load i32, ptr %9, align 8
  %11 = load i32, ptr %6, align 4
  %12 = sub nsw i32 %10, %11
  call void @_ZNK8MyString6substrEii(ptr dead_on_unwind writable sret(%class.MyString) align 8 %0, ptr noundef nonnull align 8 dereferenceable(16) %7, i32 noundef %8, i32 noundef %12)
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef nonnull align 8 dereferenceable(16) ptr @_ZN8MyStringpLESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %7, align 8
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %8, align 8
  store ptr %0, ptr %5, align 8
  %9 = load ptr, ptr %5, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %4, i64 16, i1 false)
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  %11 = load i64, ptr %10, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  %13 = load ptr, ptr %12, align 8
  call void @_ZN8MyString6appendESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %9, i64 %11, ptr %13)
  ret ptr %9
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local void @_ZNK8MyStringplESt17basic_string_viewIcSt11char_traitsIcEE(ptr dead_on_unwind noalias writable sret(%class.MyString) align 8 %0, ptr noundef nonnull align 8 dereferenceable(16) %1, i64 %2, ptr %3) #0 align 2 personality ptr @__gxx_personality_v0 {
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = alloca ptr, align 8
  %8 = alloca %class.MyString, align 8
  %9 = alloca %"class.std::basic_string_view", align 8
  %10 = alloca ptr, align 8
  %11 = alloca i32, align 4
  store ptr %0, ptr %5, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  store i64 %2, ptr %12, align 8
  %13 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  store ptr %3, ptr %13, align 8
  store ptr %1, ptr %7, align 8
  %14 = load ptr, ptr %7, align 8
  call void @_ZN8MyStringC1ERKS_(ptr noundef nonnull align 8 dereferenceable(16) %8, ptr noundef nonnull align 8 dereferenceable(16) %14)
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %9, ptr align 8 %6, i64 16, i1 false)
  %15 = getelementptr inbounds nuw { i64, ptr }, ptr %9, i32 0, i32 0
  %16 = load i64, ptr %15, align 8
  %17 = getelementptr inbounds nuw { i64, ptr }, ptr %9, i32 0, i32 1
  %18 = load ptr, ptr %17, align 8
  %19 = invoke noundef nonnull align 8 dereferenceable(16) ptr @_ZN8MyStringpLESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %8, i64 %16, ptr %18)
          to label %20 unwind label %22

20:                                               ; preds = %4
  invoke void @_ZN8MyStringC1ERKS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %19)
          to label %21 unwind label %22

21:                                               ; preds = %20
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %8) #12
  ret void

22:                                               ; preds = %20, %4
  %23 = landingpad { ptr, i32 }
          cleanup
  %24 = extractvalue { ptr, i32 } %23, 0
  store ptr %24, ptr %10, align 8
  %25 = extractvalue { ptr, i32 } %23, 1
  store i32 %25, ptr %11, align 4
  call void @_ZN8MyStringD1Ev(ptr noundef nonnull align 8 dereferenceable(16) %8) #12
  br label %26

26:                                               ; preds = %22
  %27 = load ptr, ptr %10, align 8
  %28 = load i32, ptr %11, align 4
  %29 = insertvalue { ptr, i32 } poison, ptr %27, 0
  %30 = insertvalue { ptr, i32 } %29, i32 %28, 1
  resume { ptr, i32 } %30
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef nonnull align 1 dereferenceable(1) ptr @_ZNK8MyStringixEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %5 = load ptr, ptr %3, align 8
  %6 = getelementptr inbounds nuw %class.MyString, ptr %5, i32 0, i32 0
  %7 = load ptr, ptr %6, align 8
  %8 = load i32, ptr %4, align 4
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds i8, ptr %7, i64 %9
  ret ptr %10
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca i16, align 2
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  %8 = load ptr, ptr %4, align 8
  %9 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 1
  %10 = load i32, ptr %9, align 8
  store i32 %10, ptr %6, align 4
  %11 = load i32, ptr %6, align 4
  %12 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  %13 = getelementptr inbounds nuw %class.MyString, ptr %12, i32 0, i32 1
  %14 = load i32, ptr %13, align 8
  %15 = icmp slt i32 %11, %14
  br i1 %15, label %16, label %17

16:                                               ; preds = %2
  store i16 -1, ptr %3, align 2
  br label %71

17:                                               ; preds = %2
  %18 = load i32, ptr %6, align 4
  %19 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  %20 = getelementptr inbounds nuw %class.MyString, ptr %19, i32 0, i32 1
  %21 = load i32, ptr %20, align 8
  %22 = icmp sgt i32 %18, %21
  br i1 %22, label %23, label %24

23:                                               ; preds = %17
  store i16 1, ptr %3, align 2
  br label %71

24:                                               ; preds = %17
  store i32 0, ptr %7, align 4
  br label %25

25:                                               ; preds = %67, %24
  %26 = load i32, ptr %7, align 4
  %27 = load i32, ptr %6, align 4
  %28 = icmp slt i32 %26, %27
  br i1 %28, label %29, label %70

29:                                               ; preds = %25
  %30 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  %31 = load ptr, ptr %30, align 8
  %32 = load i32, ptr %7, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds i8, ptr %31, i64 %33
  %35 = load i8, ptr %34, align 1
  %36 = sext i8 %35 to i32
  %37 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  %38 = getelementptr inbounds nuw %class.MyString, ptr %37, i32 0, i32 0
  %39 = load ptr, ptr %38, align 8
  %40 = load i32, ptr %7, align 4
  %41 = sext i32 %40 to i64
  %42 = getelementptr inbounds i8, ptr %39, i64 %41
  %43 = load i8, ptr %42, align 1
  %44 = sext i8 %43 to i32
  %45 = icmp ne i32 %36, %44
  br i1 %45, label %46, label %66

46:                                               ; preds = %29
  %47 = getelementptr inbounds nuw %class.MyString, ptr %8, i32 0, i32 0
  %48 = load ptr, ptr %47, align 8
  %49 = load i32, ptr %7, align 4
  %50 = sext i32 %49 to i64
  %51 = getelementptr inbounds i8, ptr %48, i64 %50
  %52 = load i8, ptr %51, align 1
  %53 = sext i8 %52 to i32
  %54 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  %55 = getelementptr inbounds nuw %class.MyString, ptr %54, i32 0, i32 0
  %56 = load ptr, ptr %55, align 8
  %57 = load i32, ptr %7, align 4
  %58 = sext i32 %57 to i64
  %59 = getelementptr inbounds i8, ptr %56, i64 %58
  %60 = load i8, ptr %59, align 1
  %61 = sext i8 %60 to i32
  %62 = sub nsw i32 %53, %61
  %63 = icmp sgt i32 %62, 0
  br i1 %63, label %64, label %65

64:                                               ; preds = %46
  store i16 1, ptr %3, align 2
  br label %71

65:                                               ; preds = %46
  store i16 -1, ptr %3, align 2
  br label %71

66:                                               ; preds = %29
  br label %67

67:                                               ; preds = %66
  %68 = load i32, ptr %7, align 4
  %69 = add nsw i32 %68, 1
  store i32 %69, ptr %7, align 4
  br label %25, !llvm.loop !13

70:                                               ; preds = %25
  store i16 0, ptr %3, align 2
  br label %71

71:                                               ; preds = %70, %65, %64, %23, %16
  %72 = load i16, ptr %3, align 2
  ret i16 %72
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringgtERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp eq i32 %8, 1
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringltERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp eq i32 %8, -1
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringgeERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp ne i32 %8, -1
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringleERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp ne i32 %8, 1
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringneERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp ne i32 %8, 0
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef zeroext i1 @_ZNK8MyStringeqERS_(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 align 2 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef signext i16 @_ZNK8MyString7compareERS_(ptr noundef nonnull align 8 dereferenceable(16) %5, ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = sext i16 %7 to i32
  %9 = icmp eq i32 %8, 0
  ret i1 %9
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef nonnull align 8 dereferenceable(256) ptr @_ZrsRSt14basic_ifstreamIcSt11char_traitsIcEER8MyString(ptr noundef nonnull align 8 dereferenceable(256) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #1 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  call void @llvm.trap()
  unreachable
}

; Function Attrs: cold noreturn nounwind memory(inaccessiblemem: write)
declare void @llvm.trap() #8

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef nonnull align 8 dereferenceable(248) ptr @_ZlsRSt14basic_ofstreamIcSt11char_traitsIcEERK8MyString(ptr noundef nonnull align 8 dereferenceable(248) %0, ptr noundef nonnull align 8 dereferenceable(16) %1) #0 {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  store ptr %0, ptr %3, align 8
  store ptr %1, ptr %4, align 8
  %5 = load ptr, ptr %3, align 8, !nonnull !8, !align !9
  %6 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %7 = call noundef ptr @_ZNK8MyString5c_strEv(ptr noundef nonnull align 8 dereferenceable(16) %6)
  %8 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %9 = call noundef i32 @_ZNK8MyString4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %8)
  %10 = sext i32 %9 to i64
  %11 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZNSo5writeEPKcl(ptr noundef nonnull align 8 dereferenceable(8) %5, ptr noundef %7, i64 noundef %10)
  %12 = load ptr, ptr %3, align 8, !nonnull !8, !align !9
  ret ptr %12
}

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSo5writeEPKcl(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef, i64 noundef) #5

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef i32 @_ZNK8MyString4findESt17basic_string_viewIcSt11char_traitsIcEE(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2) #0 align 2 {
  %4 = alloca %"class.std::basic_string_view", align 8
  %5 = alloca ptr, align 8
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 0
  store i64 %1, ptr %7, align 8
  %8 = getelementptr inbounds nuw { i64, ptr }, ptr %4, i32 0, i32 1
  store ptr %2, ptr %8, align 8
  store ptr %0, ptr %5, align 8
  %9 = load ptr, ptr %5, align 8
  call void @llvm.memcpy.p0.p0.i64(ptr align 8 %6, ptr align 8 %4, i64 16, i1 false)
  %10 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  %11 = load i64, ptr %10, align 8
  %12 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  %13 = load ptr, ptr %12, align 8
  %14 = call noundef i32 @_ZNK8MyString4findESt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %9, i64 %11, ptr %13, i32 noundef 0)
  ret i32 %14
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef i32 @_ZNK8MyString4findESt17basic_string_viewIcSt11char_traitsIcEEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i64 %1, ptr %2, i32 noundef %3) #0 align 2 personality ptr @__gxx_personality_v0 {
  %5 = alloca i32, align 4
  %6 = alloca %"class.std::basic_string_view", align 8
  %7 = alloca ptr, align 8
  %8 = alloca i32, align 4
  %9 = alloca ptr, align 8
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca ptr, align 8
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 0
  store i64 %1, ptr %15, align 8
  %16 = getelementptr inbounds nuw { i64, ptr }, ptr %6, i32 0, i32 1
  store ptr %2, ptr %16, align 8
  store ptr %0, ptr %7, align 8
  store i32 %3, ptr %8, align 4
  %17 = load ptr, ptr %7, align 8
  %18 = load i32, ptr %8, align 4
  %19 = icmp slt i32 %18, 0
  br i1 %19, label %20, label %25

20:                                               ; preds = %4
  %21 = getelementptr inbounds nuw %class.MyString, ptr %17, i32 0, i32 2
  %22 = load i32, ptr %21, align 4
  %23 = load i32, ptr %8, align 4
  %24 = sub nsw i32 %22, %23
  store i32 %24, ptr %8, align 4
  br label %25

25:                                               ; preds = %20, %4
  %26 = load i32, ptr %8, align 4
  %27 = getelementptr inbounds nuw %class.MyString, ptr %17, i32 0, i32 2
  %28 = load i32, ptr %27, align 4
  %29 = icmp sgt i32 %26, %28
  br i1 %29, label %33, label %30

30:                                               ; preds = %25
  %31 = load i32, ptr %8, align 4
  %32 = icmp slt i32 %31, 0
  br i1 %32, label %33, label %40

33:                                               ; preds = %30, %25
  %34 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %34, ptr noundef @.str)
          to label %35 unwind label %36

35:                                               ; preds = %33
  call void @__cxa_throw(ptr %34, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

36:                                               ; preds = %33
  %37 = landingpad { ptr, i32 }
          cleanup
  %38 = extractvalue { ptr, i32 } %37, 0
  store ptr %38, ptr %9, align 8
  %39 = extractvalue { ptr, i32 } %37, 1
  store i32 %39, ptr %10, align 4
  call void @__cxa_free_exception(ptr %34) #12
  br label %96

40:                                               ; preds = %30
  %41 = call noundef i64 @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  %42 = trunc i64 %41 to i32
  store i32 %42, ptr %11, align 4
  %43 = load i32, ptr %11, align 4
  %44 = icmp eq i32 %43, 0
  br i1 %44, label %45, label %46

45:                                               ; preds = %40
  store i32 -1, ptr %5, align 4
  br label %94

46:                                               ; preds = %40
  %47 = call noundef ptr @_ZNKSt17basic_string_viewIcSt11char_traitsIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(16) %6) #12
  store ptr %47, ptr %12, align 8
  %48 = load i32, ptr %8, align 4
  store i32 %48, ptr %13, align 4
  br label %49

49:                                               ; preds = %90, %46
  %50 = load i32, ptr %13, align 4
  %51 = getelementptr inbounds nuw %class.MyString, ptr %17, i32 0, i32 1
  %52 = load i32, ptr %51, align 8
  %53 = load i32, ptr %11, align 4
  %54 = sub nsw i32 %52, %53
  %55 = icmp sle i32 %50, %54
  br i1 %55, label %56, label %93

56:                                               ; preds = %49
  store i32 0, ptr %14, align 4
  br label %57

57:                                               ; preds = %80, %56
  %58 = load i32, ptr %14, align 4
  %59 = load i32, ptr %11, align 4
  %60 = icmp slt i32 %58, %59
  br i1 %60, label %61, label %78

61:                                               ; preds = %57
  %62 = getelementptr inbounds nuw %class.MyString, ptr %17, i32 0, i32 0
  %63 = load ptr, ptr %62, align 8
  %64 = load i32, ptr %13, align 4
  %65 = load i32, ptr %14, align 4
  %66 = add nsw i32 %64, %65
  %67 = sext i32 %66 to i64
  %68 = getelementptr inbounds i8, ptr %63, i64 %67
  %69 = load i8, ptr %68, align 1
  %70 = sext i8 %69 to i32
  %71 = load ptr, ptr %12, align 8
  %72 = load i32, ptr %14, align 4
  %73 = sext i32 %72 to i64
  %74 = getelementptr inbounds i8, ptr %71, i64 %73
  %75 = load i8, ptr %74, align 1
  %76 = sext i8 %75 to i32
  %77 = icmp eq i32 %70, %76
  br label %78

78:                                               ; preds = %61, %57
  %79 = phi i1 [ false, %57 ], [ %77, %61 ]
  br i1 %79, label %80, label %83

80:                                               ; preds = %78
  %81 = load i32, ptr %14, align 4
  %82 = add nsw i32 %81, 1
  store i32 %82, ptr %14, align 4
  br label %57, !llvm.loop !14

83:                                               ; preds = %78
  %84 = load i32, ptr %14, align 4
  %85 = load i32, ptr %11, align 4
  %86 = icmp eq i32 %84, %85
  br i1 %86, label %87, label %89

87:                                               ; preds = %83
  %88 = load i32, ptr %13, align 4
  store i32 %88, ptr %5, align 4
  br label %94

89:                                               ; preds = %83
  br label %90

90:                                               ; preds = %89
  %91 = load i32, ptr %13, align 4
  %92 = add nsw i32 %91, 1
  store i32 %92, ptr %13, align 4
  br label %49, !llvm.loop !15

93:                                               ; preds = %49
  store i32 -1, ptr %5, align 4
  br label %94

94:                                               ; preds = %93, %87, %45
  %95 = load i32, ptr %5, align 4
  ret i32 %95

96:                                               ; preds = %36
  %97 = load ptr, ptr %9, align 8
  %98 = load i32, ptr %10, align 4
  %99 = insertvalue { ptr, i32 } poison, ptr %97, 0
  %100 = insertvalue { ptr, i32 } %99, i32 %98, 1
  resume { ptr, i32 } %100
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef signext i8 @_ZN8MyString2atEi(ptr noundef nonnull align 8 dereferenceable(16) %0, i32 noundef %1) #0 align 2 personality ptr @__gxx_personality_v0 {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  %5 = alloca ptr, align 8
  %6 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %7 = load ptr, ptr %3, align 8
  %8 = load i32, ptr %4, align 4
  %9 = icmp slt i32 %8, 0
  br i1 %9, label %10, label %15

10:                                               ; preds = %2
  %11 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  %12 = load i32, ptr %11, align 4
  %13 = load i32, ptr %4, align 4
  %14 = sub nsw i32 %12, %13
  store i32 %14, ptr %4, align 4
  br label %15

15:                                               ; preds = %10, %2
  %16 = load i32, ptr %4, align 4
  %17 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 2
  %18 = load i32, ptr %17, align 4
  %19 = icmp sgt i32 %16, %18
  br i1 %19, label %23, label %20

20:                                               ; preds = %15
  %21 = load i32, ptr %4, align 4
  %22 = icmp slt i32 %21, 0
  br i1 %22, label %23, label %30

23:                                               ; preds = %20, %15
  %24 = call ptr @__cxa_allocate_exception(i64 16) #12
  invoke void @_ZNSt12out_of_rangeC1EPKc(ptr noundef nonnull align 8 dereferenceable(16) %24, ptr noundef @.str)
          to label %25 unwind label %26

25:                                               ; preds = %23
  call void @__cxa_throw(ptr %24, ptr @_ZTISt12out_of_range, ptr @_ZNSt12out_of_rangeD1Ev) #15
  unreachable

26:                                               ; preds = %23
  %27 = landingpad { ptr, i32 }
          cleanup
  %28 = extractvalue { ptr, i32 } %27, 0
  store ptr %28, ptr %5, align 8
  %29 = extractvalue { ptr, i32 } %27, 1
  store i32 %29, ptr %6, align 4
  call void @__cxa_free_exception(ptr %24) #12
  br label %37

30:                                               ; preds = %20
  %31 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %32 = load ptr, ptr %31, align 8
  %33 = load i32, ptr %4, align 4
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds i8, ptr %32, i64 %34
  %36 = load i8, ptr %35, align 1
  ret i8 %36

37:                                               ; preds = %26
  %38 = load ptr, ptr %5, align 8
  %39 = load i32, ptr %6, align 4
  %40 = insertvalue { ptr, i32 } poison, ptr %38, 0
  %41 = insertvalue { ptr, i32 } %40, i32 %39, 1
  resume { ptr, i32 } %41
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define dso_local noundef i32 @_ZN8MyString6to_intEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #0 align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = call noundef float @_ZN8MyString8to_floatEv(ptr noundef nonnull align 8 dereferenceable(16) %3)
  %5 = fptosi float %4 to i32
  ret i32 %5
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define dso_local noundef float @_ZN8MyString8to_floatEv(ptr noundef nonnull align 8 dereferenceable(16) %0) #1 align 2 {
  %2 = alloca ptr, align 8
  %3 = alloca float, align 4
  %4 = alloca float, align 4
  %5 = alloca i32, align 4
  %6 = alloca float, align 4
  store ptr %0, ptr %2, align 8
  %7 = load ptr, ptr %2, align 8
  store float 0.000000e+00, ptr %3, align 4
  store float 1.000000e+00, ptr %4, align 4
  store i32 0, ptr %5, align 4
  %8 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %9 = load ptr, ptr %8, align 8
  %10 = load i32, ptr %5, align 4
  %11 = sext i32 %10 to i64
  %12 = getelementptr inbounds i8, ptr %9, i64 %11
  %13 = load i8, ptr %12, align 1
  %14 = sext i8 %13 to i32
  %15 = icmp eq i32 %14, 45
  br i1 %15, label %16, label %19

16:                                               ; preds = %1
  store float -1.000000e+00, ptr %4, align 4
  %17 = load i32, ptr %5, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, ptr %5, align 4
  br label %32

19:                                               ; preds = %1
  %20 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %21 = load ptr, ptr %20, align 8
  %22 = load i32, ptr %5, align 4
  %23 = sext i32 %22 to i64
  %24 = getelementptr inbounds i8, ptr %21, i64 %23
  %25 = load i8, ptr %24, align 1
  %26 = sext i8 %25 to i32
  %27 = icmp eq i32 %26, 43
  br i1 %27, label %28, label %31

28:                                               ; preds = %19
  %29 = load i32, ptr %5, align 4
  %30 = add nsw i32 %29, 1
  store i32 %30, ptr %5, align 4
  br label %31

31:                                               ; preds = %28, %19
  br label %32

32:                                               ; preds = %31, %16
  br label %33

33:                                               ; preds = %58, %32
  %34 = load i32, ptr %5, align 4
  %35 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %36 = load i32, ptr %35, align 8
  %37 = icmp slt i32 %34, %36
  br i1 %37, label %38, label %56

38:                                               ; preds = %33
  %39 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %40 = load ptr, ptr %39, align 8
  %41 = load i32, ptr %5, align 4
  %42 = sext i32 %41 to i64
  %43 = getelementptr inbounds i8, ptr %40, i64 %42
  %44 = load i8, ptr %43, align 1
  %45 = sext i8 %44 to i32
  %46 = icmp sge i32 %45, 48
  br i1 %46, label %47, label %56

47:                                               ; preds = %38
  %48 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %49 = load ptr, ptr %48, align 8
  %50 = load i32, ptr %5, align 4
  %51 = sext i32 %50 to i64
  %52 = getelementptr inbounds i8, ptr %49, i64 %51
  %53 = load i8, ptr %52, align 1
  %54 = sext i8 %53 to i32
  %55 = icmp sle i32 %54, 57
  br label %56

56:                                               ; preds = %47, %38, %33
  %57 = phi i1 [ false, %38 ], [ false, %33 ], [ %55, %47 ]
  br i1 %57, label %58, label %72

58:                                               ; preds = %56
  %59 = load float, ptr %3, align 4
  %60 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %61 = load ptr, ptr %60, align 8
  %62 = load i32, ptr %5, align 4
  %63 = sext i32 %62 to i64
  %64 = getelementptr inbounds i8, ptr %61, i64 %63
  %65 = load i8, ptr %64, align 1
  %66 = sext i8 %65 to i32
  %67 = sub nsw i32 %66, 48
  %68 = sitofp i32 %67 to float
  %69 = call float @llvm.fmuladd.f32(float %59, float 1.000000e+01, float %68)
  store float %69, ptr %3, align 4
  %70 = load i32, ptr %5, align 4
  %71 = add nsw i32 %70, 1
  store i32 %71, ptr %5, align 4
  br label %33, !llvm.loop !16

72:                                               ; preds = %56
  %73 = load i32, ptr %5, align 4
  %74 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %75 = load i32, ptr %74, align 8
  %76 = icmp slt i32 %73, %75
  br i1 %76, label %77, label %132

77:                                               ; preds = %72
  %78 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %79 = load ptr, ptr %78, align 8
  %80 = load i32, ptr %5, align 4
  %81 = sext i32 %80 to i64
  %82 = getelementptr inbounds i8, ptr %79, i64 %81
  %83 = load i8, ptr %82, align 1
  %84 = sext i8 %83 to i32
  %85 = icmp eq i32 %84, 46
  br i1 %85, label %86, label %132

86:                                               ; preds = %77
  %87 = load i32, ptr %5, align 4
  %88 = add nsw i32 %87, 1
  store i32 %88, ptr %5, align 4
  store float 0x3FB99999A0000000, ptr %6, align 4
  br label %89

89:                                               ; preds = %114, %86
  %90 = load i32, ptr %5, align 4
  %91 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 1
  %92 = load i32, ptr %91, align 8
  %93 = icmp slt i32 %90, %92
  br i1 %93, label %94, label %112

94:                                               ; preds = %89
  %95 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %96 = load ptr, ptr %95, align 8
  %97 = load i32, ptr %5, align 4
  %98 = sext i32 %97 to i64
  %99 = getelementptr inbounds i8, ptr %96, i64 %98
  %100 = load i8, ptr %99, align 1
  %101 = sext i8 %100 to i32
  %102 = icmp sge i32 %101, 48
  br i1 %102, label %103, label %112

103:                                              ; preds = %94
  %104 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %105 = load ptr, ptr %104, align 8
  %106 = load i32, ptr %5, align 4
  %107 = sext i32 %106 to i64
  %108 = getelementptr inbounds i8, ptr %105, i64 %107
  %109 = load i8, ptr %108, align 1
  %110 = sext i8 %109 to i32
  %111 = icmp sle i32 %110, 57
  br label %112

112:                                              ; preds = %103, %94, %89
  %113 = phi i1 [ false, %94 ], [ false, %89 ], [ %111, %103 ]
  br i1 %113, label %114, label %131

114:                                              ; preds = %112
  %115 = getelementptr inbounds nuw %class.MyString, ptr %7, i32 0, i32 0
  %116 = load ptr, ptr %115, align 8
  %117 = load i32, ptr %5, align 4
  %118 = sext i32 %117 to i64
  %119 = getelementptr inbounds i8, ptr %116, i64 %118
  %120 = load i8, ptr %119, align 1
  %121 = sext i8 %120 to i32
  %122 = sub nsw i32 %121, 48
  %123 = sitofp i32 %122 to float
  %124 = load float, ptr %6, align 4
  %125 = load float, ptr %3, align 4
  %126 = call float @llvm.fmuladd.f32(float %123, float %124, float %125)
  store float %126, ptr %3, align 4
  %127 = load float, ptr %6, align 4
  %128 = fdiv float %127, 1.000000e+01
  store float %128, ptr %6, align 4
  %129 = load i32, ptr %5, align 4
  %130 = add nsw i32 %129, 1
  store i32 %130, ptr %5, align 4
  br label %89, !llvm.loop !17

131:                                              ; preds = %112
  br label %132

132:                                              ; preds = %131, %77, %72
  %133 = load float, ptr %3, align 4
  %134 = load float, ptr %4, align 4
  %135 = fmul float %133, %134
  ret float %135
}

; Function Attrs: nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.fmuladd.f32(float, float, float) #9

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local void @_ZNSt17basic_string_viewIcSt11char_traitsIcEEC2EPKcm(ptr noundef nonnull align 8 dereferenceable(16) %0, ptr noundef %1, i64 noundef %2) unnamed_addr #1 comdat align 2 {
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i64, align 8
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  store i64 %2, ptr %6, align 8
  %7 = load ptr, ptr %4, align 8
  %8 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %7, i32 0, i32 0
  %9 = load i64, ptr %6, align 8
  store i64 %9, ptr %8, align 8
  %10 = getelementptr inbounds nuw %"class.std::basic_string_view", ptr %7, i32 0, i32 1
  %11 = load ptr, ptr %5, align 8
  store ptr %11, ptr %10, align 8
  ret void
}

; Function Attrs: mustprogress noinline optnone sspstrong uwtable
define linkonce_odr dso_local noundef i64 @_ZSt10__sv_checkmmPKc(i64 noundef %0, i64 noundef %1, ptr noundef %2) #0 comdat {
  %4 = alloca i64, align 8
  %5 = alloca i64, align 8
  %6 = alloca ptr, align 8
  store i64 %0, ptr %4, align 8
  store i64 %1, ptr %5, align 8
  store ptr %2, ptr %6, align 8
  %7 = load i64, ptr %5, align 8
  %8 = load i64, ptr %4, align 8
  %9 = icmp ugt i64 %7, %8
  br i1 %9, label %10, label %14

10:                                               ; preds = %3
  %11 = load ptr, ptr %6, align 8
  %12 = load i64, ptr %5, align 8
  %13 = load i64, ptr %4, align 8
  call void (ptr, ...) @_ZSt24__throw_out_of_range_fmtPKcz(ptr noundef @.str.8, ptr noundef %11, i64 noundef %12, i64 noundef %13) #17
  unreachable

14:                                               ; preds = %3
  %15 = load i64, ptr %5, align 8
  ret i64 %15
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3minImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8) %0, ptr noundef nonnull align 8 dereferenceable(8) %1) #1 comdat {
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  store ptr %0, ptr %4, align 8
  store ptr %1, ptr %5, align 8
  %6 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  %7 = load i64, ptr %6, align 8
  %8 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  %9 = load i64, ptr %8, align 8
  %10 = icmp ult i64 %7, %9
  br i1 %10, label %11, label %13

11:                                               ; preds = %2
  %12 = load ptr, ptr %5, align 8, !nonnull !8, !align !9
  store ptr %12, ptr %3, align 8
  br label %15

13:                                               ; preds = %2
  %14 = load ptr, ptr %4, align 8, !nonnull !8, !align !9
  store ptr %14, ptr %3, align 8
  br label %15

15:                                               ; preds = %13, %11
  %16 = load ptr, ptr %3, align 8
  ret ptr %16
}

; Function Attrs: cold noreturn
declare void @_ZSt24__throw_out_of_range_fmtPKcz(ptr noundef, ...) #10

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef i64 @_ZNSt11char_traitsIcE6lengthEPKc(ptr noundef %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = call i64 @strlen(ptr noundef %3) #12
  ret i64 %4
}

; Function Attrs: noinline noreturn nounwind sspstrong uwtable
define linkonce_odr hidden void @__clang_call_terminate(ptr noundef %0) #11 comdat {
  %2 = call ptr @__cxa_begin_catch(ptr %0) #12
  call void @_ZSt9terminatev() #16
  unreachable
}

declare ptr @__cxa_begin_catch(ptr)

declare void @_ZSt9terminatev()

; Function Attrs: nounwind
declare i64 @strlen(ptr noundef) #6

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4dataEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 personality ptr @__gxx_personality_v0 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = invoke noundef ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEv(ptr noundef nonnull align 8 dereferenceable(32) %3)
          to label %5 unwind label %6

5:                                                ; preds = %1
  ret ptr %4

6:                                                ; preds = %1
  %7 = landingpad { ptr, i32 }
          catch ptr null
  %8 = extractvalue { ptr, i32 } %7, 0
  call void @__clang_call_terminate(ptr %8) #16
  unreachable
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4sizeEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  %3 = alloca i64, align 8
  store ptr %0, ptr %2, align 8
  %4 = load ptr, ptr %2, align 8
  %5 = getelementptr inbounds nuw %"class.std::__cxx11::basic_string", ptr %4, i32 0, i32 1
  %6 = load i64, ptr %5, align 8
  store i64 %6, ptr %3, align 8
  %7 = load i64, ptr %3, align 8
  %8 = call noundef i64 @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE8max_sizeEv(ptr noundef nonnull align 8 dereferenceable(32) %4) #12
  %9 = icmp ugt i64 %7, %8
  br i1 %9, label %10, label %11

10:                                               ; preds = %1
  unreachable

11:                                               ; preds = %1
  %12 = load i64, ptr %3, align 8
  ret i64 %12
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %"class.std::__cxx11::basic_string", ptr %3, i32 0, i32 0
  %5 = getelementptr inbounds nuw %"struct.std::__cxx11::basic_string<char>::_Alloc_hider", ptr %4, i32 0, i32 0
  %6 = load ptr, ptr %5, align 8
  ret ptr %6
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef i64 @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE8max_sizeEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 personality ptr @__gxx_personality_v0 {
  %2 = alloca ptr, align 8
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca ptr, align 8
  %6 = alloca i64, align 8
  %7 = alloca i64, align 8
  store ptr %0, ptr %5, align 8
  %8 = load ptr, ptr %5, align 8
  store i64 9223372036854775807, ptr %6, align 8
  %9 = invoke noundef nonnull align 1 dereferenceable(1) ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_get_allocatorEv(ptr noundef nonnull align 8 dereferenceable(32) %8)
          to label %10 unwind label %17

10:                                               ; preds = %1
  store ptr %9, ptr %4, align 8
  %11 = load ptr, ptr %4, align 8, !nonnull !8
  store ptr %11, ptr %3, align 8
  %12 = load ptr, ptr %3, align 8
  store ptr %12, ptr %2, align 8
  %13 = load ptr, ptr %2, align 8
  store i64 9223372036854775807, ptr %7, align 8
  %14 = call noundef nonnull align 8 dereferenceable(8) ptr @_ZSt3minImERKT_S2_S2_(ptr noundef nonnull align 8 dereferenceable(8) %6, ptr noundef nonnull align 8 dereferenceable(8) %7)
  %15 = load i64, ptr %14, align 8
  %16 = sub i64 %15, 1
  ret i64 %16

17:                                               ; preds = %1
  %18 = landingpad { ptr, i32 }
          catch ptr null
  %19 = extractvalue { ptr, i32 } %18, 0
  call void @__clang_call_terminate(ptr %19) #16
  unreachable
}

; Function Attrs: mustprogress noinline nounwind optnone sspstrong uwtable
define linkonce_odr dso_local noundef nonnull align 1 dereferenceable(1) ptr @_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_get_allocatorEv(ptr noundef nonnull align 8 dereferenceable(32) %0) #1 comdat align 2 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds nuw %"class.std::__cxx11::basic_string", ptr %3, i32 0, i32 0
  ret ptr %4
}

attributes #0 = { mustprogress noinline optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nobuiltin allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #4 = { nobuiltin nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nounwind "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #8 = { cold noreturn nounwind memory(inaccessiblemem: write) }
attributes #9 = { nocallback nocreateundeforpoison nofree nosync nounwind speculatable willreturn memory(none) }
attributes #10 = { cold noreturn "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { noinline noreturn nounwind sspstrong uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #12 = { nounwind }
attributes #13 = { builtin allocsize(0) }
attributes #14 = { builtin nounwind }
attributes #15 = { noreturn }
attributes #16 = { noreturn nounwind }
attributes #17 = { cold noreturn }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 22.1.8"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = !{}
!9 = !{i64 8}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
!17 = distinct !{!17, !7}
