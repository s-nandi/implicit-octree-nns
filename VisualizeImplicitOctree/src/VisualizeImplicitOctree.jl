module VisualizeImplicitOctree

using Plots

struct Point2
    x::Float64
    y::Float64
end

struct Point3 
    x::Float64
    y::Float64
    z::Float64
end

struct Line{T}
    a::T
    b::T 
end

include("GeometryPlotter.jl")
include("ParseUtils.jl")
include("OctvisParser.jl")
include("AnalysisPlotting.jl")
include("BenchmarkPlotting.jl")

export filetoscene, save_frame, animate_frames, visualize_frame
export plot_distribution, plot_benchmark

# #dimensions
# octree_cell is_leaf? min_bound_1 max_bound_1 :. min_bound_dim max_bound_dim #points pt1 : ptk
# octree_cell ...
# ...
# end_frame
# octree_cell ...
# ...
# end_frame
# end_build (for now, just terminate here)
#
# query_point pt_coordinates #depth #status (too deep/shallow, just right) bonds_1 :. bounds_2^dim [for query box]

end # module
