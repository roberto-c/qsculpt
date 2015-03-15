; ModuleID = 'Subdivision.cl'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a64:64:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-applecl-macosx10.8.2"

%struct.Surface = type { i32, i32, i32, %union.anon, %union.anon.0, %union.anon.1 }
%union.anon = type { i64 }
%union.anon.0 = type { i64 }
%union.anon.1 = type { i64 }
%struct.Vertex = type { <4 x float>, <4 x float>, i32 }
%struct.Edge = type { i32, i32, i32, i32 }
%struct.Face = type { i32, i32, i32 }

@sgv = internal constant [8 x i8] c"2202020\00"
@fgv = internal constant [0 x i8] zeroinitializer
@lvgv = internal constant [0 x i8*] zeroinitializer
@sgv1 = internal constant [3 x i8] c"12\00"
@fgv2 = internal constant [0 x i8] zeroinitializer
@lvgv3 = internal constant [0 x i8*] zeroinitializer
@sgv4 = internal constant [3 x i8] c"12\00"
@fgv5 = internal constant [0 x i8] zeroinitializer
@lvgv6 = internal constant [0 x i8*] zeroinitializer
@llvm.global.annotations = appending global [3 x { i8*, i8*, i8*, i8*, i32 }] [{ i8*, i8*, i8*, i8*, i32 } { i8* bitcast (void (%struct.Surface addrspace(1)*, %struct.Vertex addrspace(1)*, i32, %struct.Edge addrspace(1)*, i32, %struct.Face addrspace(1)*, i32)* @surface_init to i8*), i8* getelementptr inbounds ([8 x i8]* @sgv, i32 0, i32 0), i8* getelementptr inbounds ([0 x i8]* @fgv, i32 0, i32 0), i8* bitcast ([0 x i8*]* @lvgv to i8*), i32 0 }, { i8*, i8*, i8*, i8*, i32 } { i8* bitcast (void (%struct.Surface addrspace(1)*, %struct.Surface addrspace(1)*)* @subdivide_edges to i8*), i8* getelementptr inbounds ([3 x i8]* @sgv1, i32 0, i32 0), i8* getelementptr inbounds ([0 x i8]* @fgv2, i32 0, i32 0), i8* bitcast ([0 x i8*]* @lvgv3 to i8*), i32 0 }, { i8*, i8*, i8*, i8*, i32 } { i8* bitcast (void (%struct.Surface addrspace(1)*, %struct.Surface addrspace(1)*)* @subdivide_faces to i8*), i8* getelementptr inbounds ([3 x i8]* @sgv4, i32 0, i32 0), i8* getelementptr inbounds ([0 x i8]* @fgv5, i32 0, i32 0), i8* bitcast ([0 x i8*]* @lvgv6 to i8*), i32 0 }], section "llvm.metadata"

define void @surface_init(%struct.Surface addrspace(1)* nocapture %surface, %struct.Vertex addrspace(1)* %vtxList, i32 %num_vertices, %struct.Edge addrspace(1)* %edgeList, i32 %num_edges, %struct.Face addrspace(1)* %faceList, i32 %num_faces) nounwind uwtable {
  %1 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 0
  store i32 %num_vertices, i32 addrspace(1)* %1, align 4, !tbaa !3
  %2 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 1
  store i32 %num_edges, i32 addrspace(1)* %2, align 4, !tbaa !3
  %3 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 2
  store i32 %num_faces, i32 addrspace(1)* %3, align 4, !tbaa !3
  %4 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 3, i32 0
  %vtxList.c = ptrtoint %struct.Vertex addrspace(1)* %vtxList to i64
  store i64 %vtxList.c, i64 addrspace(1)* %4, align 8, !tbaa !6
  %5 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 4, i32 0
  %edgeList.c = ptrtoint %struct.Edge addrspace(1)* %edgeList to i64
  store i64 %edgeList.c, i64 addrspace(1)* %5, align 8, !tbaa !6
  %6 = getelementptr inbounds %struct.Surface addrspace(1)* %surface, i64 0, i32 5, i32 0
  %faceList.c = ptrtoint %struct.Face addrspace(1)* %faceList to i64
  store i64 %faceList.c, i64 addrspace(1)* %6, align 8, !tbaa !6
  ret void
}

define void @subdivide_edges(%struct.Surface addrspace(1)* nocapture %inpSurface, %struct.Surface addrspace(1)* nocapture %outSurface) nounwind uwtable {
  %1 = tail call i64 @get_global_id(i32 0) nounwind
  %2 = trunc i64 %1 to i32
  %3 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 1
  %4 = load i32 addrspace(1)* %3, align 4, !tbaa !3
  %5 = icmp ult i32 %2, %4
  br i1 %5, label %6, label %18

; <label>:6                                       ; preds = %0
  %7 = and i64 %1, 4294967295
  %8 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 3
  %9 = bitcast %union.anon addrspace(1)* %8 to %struct.Vertex addrspace(1)* addrspace(1)*
  %10 = load %struct.Vertex addrspace(1)* addrspace(1)* %9, align 8, !tbaa !6
  %11 = getelementptr inbounds %struct.Vertex addrspace(1)* %10, i64 %7, i32 0
  %12 = load <4 x float> addrspace(1)* %11, align 16, !tbaa !4
  %13 = fadd <4 x float> %12, <float 0x3F847AE140000000, float 0.000000e+00, float 0.000000e+00, float 0.000000e+00>
  %14 = getelementptr inbounds %struct.Surface addrspace(1)* %outSurface, i64 0, i32 3
  %15 = bitcast %union.anon addrspace(1)* %14 to %struct.Vertex addrspace(1)* addrspace(1)*
  %16 = load %struct.Vertex addrspace(1)* addrspace(1)* %15, align 8, !tbaa !6
  %17 = getelementptr inbounds %struct.Vertex addrspace(1)* %16, i64 %7, i32 0
  store <4 x float> %13, <4 x float> addrspace(1)* %17, align 16, !tbaa !4
  br label %18

; <label>:18                                      ; preds = %0, %6
  ret void
}

declare i64 @get_global_id(i32)

define void @subdivide_faces(%struct.Surface addrspace(1)* nocapture %inpSurface, %struct.Surface addrspace(1)* nocapture %outSurface) nounwind uwtable {
  %1 = tail call i64 @get_global_id(i32 0) nounwind
  %2 = trunc i64 %1 to i32
  %3 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 2
  %4 = load i32 addrspace(1)* %3, align 4, !tbaa !3
  %5 = icmp ult i32 %2, %4
  br i1 %5, label %6, label %44

; <label>:6                                       ; preds = %0
  %7 = and i64 %1, 4294967295
  %8 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 5
  %9 = bitcast %union.anon.1 addrspace(1)* %8 to %struct.Face addrspace(1)* addrspace(1)*
  %10 = load %struct.Face addrspace(1)* addrspace(1)* %9, align 8, !tbaa !6
  %11 = getelementptr inbounds %struct.Face addrspace(1)* %10, i64 %7, i32 1
  %12 = load i32 addrspace(1)* %11, align 4, !tbaa !3
  %13 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 4
  %14 = bitcast %union.anon.0 addrspace(1)* %13 to %struct.Edge addrspace(1)* addrspace(1)*
  %15 = load %struct.Edge addrspace(1)* addrspace(1)* %14, align 8, !tbaa !6
  %16 = getelementptr inbounds %struct.Surface addrspace(1)* %inpSurface, i64 0, i32 3
  %17 = bitcast %union.anon addrspace(1)* %16 to %struct.Vertex addrspace(1)* addrspace(1)*
  %18 = load %struct.Vertex addrspace(1)* addrspace(1)* %17, align 8, !tbaa !6
  br label %19

; <label>:19                                      ; preds = %19, %6
  %p.0 = phi <4 x float> [ undef, %6 ], [ %25, %19 ]
  %.pn.in = phi i32 [ %12, %6 ], [ %27, %19 ]
  %.pn = zext i32 %.pn.in to i64
  %20 = getelementptr inbounds %struct.Edge addrspace(1)* %15, i64 %.pn, i32 0
  %21 = load i32 addrspace(1)* %20, align 4, !tbaa !3
  %22 = zext i32 %21 to i64
  %23 = getelementptr inbounds %struct.Vertex addrspace(1)* %18, i64 %22, i32 0
  %24 = load <4 x float> addrspace(1)* %23, align 16, !tbaa !4
  %25 = fadd <4 x float> %p.0, %24
  %26 = getelementptr inbounds %struct.Edge addrspace(1)* %15, i64 %.pn, i32 3
  %27 = load i32 addrspace(1)* %26, align 4, !tbaa !3
  %28 = icmp eq i32 %27, %12
  br i1 %28, label %29, label %19

; <label>:29                                      ; preds = %19
  %30 = getelementptr inbounds %struct.Face addrspace(1)* %10, i64 %7, i32 0
  %31 = load i32 addrspace(1)* %30, align 4, !tbaa !3
  %32 = fdiv <4 x float> %25, i32 %31, !fpaccuracy !7
  %33 = getelementptr inbounds %struct.Surface addrspace(1)* %outSurface, i64 0, i32 0
  %34 = load i32 addrspace(1)* %33, align 4, !tbaa !3
  %35 = add i32 %34, %2
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds %struct.Surface addrspace(1)* %outSurface, i64 0, i32 3
  %38 = bitcast %union.anon addrspace(1)* %37 to %struct.Vertex addrspace(1)* addrspace(1)*
  %39 = load %struct.Vertex addrspace(1)* addrspace(1)* %38, align 8, !tbaa !6
  %40 = getelementptr inbounds %struct.Vertex addrspace(1)* %39, i64 %36, i32 0
  store <4 x float> %32, <4 x float> addrspace(1)* %40, align 16, !tbaa !4
  %41 = load i32 addrspace(1)* %3, align 4, !tbaa !3
  %42 = load i32 addrspace(1)* %33, align 4, !tbaa !3
  %43 = add i32 %42, %41
  store i32 %43, i32 addrspace(1)* %33, align 4, !tbaa !3
  br label %44

; <label>:44                                      ; preds = %0, %29
  ret void
}

!opencl.kernels = !{!0, !1, !2}

!0 = metadata !{void (%struct.Surface addrspace(1)*, %struct.Vertex addrspace(1)*, i32, %struct.Edge addrspace(1)*, i32, %struct.Face addrspace(1)*, i32)* @surface_init}
!1 = metadata !{void (%struct.Surface addrspace(1)*, %struct.Surface addrspace(1)*)* @subdivide_edges}
!2 = metadata !{void (%struct.Surface addrspace(1)*, %struct.Surface addrspace(1)*)* @subdivide_faces}
!3 = metadata !{metadata !"int", metadata !4}
!4 = metadata !{metadata !"omnipotent char", metadata !5}
!5 = metadata !{metadata !"Simple C/C++ TBAA", null}
!6 = metadata !{metadata !"any pointer", metadata !4}
!7 = metadata !{i32 5, i32 2}
