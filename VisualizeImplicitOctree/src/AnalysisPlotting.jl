function parse_distribution_file(filename)
    lines = []
    open(filename, "r") do io
        append!(lines, eachline(io))
    end
    num_cells_header, num_cells = split(lines[1])
    max_depth_header, max_depth = split(lines[2])
    @assert occursin("num_cells", num_cells_header)
    @assert occursin("max_depth", max_depth_header)
    return parse(Int32, num_cells), parse(Int32, max_depth)
end

function plot_distribution_helper(directory, distname, dimensions, outdir, ext)
    num_points = Dict()
    num_cells = Dict()
    max_depths = Dict()
    max_points_in_file = Set()
    for filename in readdir(directory)
        if occursin(distname, filename)
            tokens = split(filename, "_")
            curr_dimensions = parse(Int32, tokens[2])
            npoints = parse(Int64, tokens[3])
            max_points = parse(Int32, tokens[6])

            if curr_dimensions != dimensions
                continue
            end

            ncells, mdepth = parse_distribution_file(joinpath(directory, filename))
            if max_points ∉ max_points_in_file
                push!(max_points_in_file, max_points)
                num_points[max_points] = []
                num_cells[max_points] = []
                max_depths[max_points] = []
            end
            push!(num_points[max_points], npoints)
            push!(num_cells[max_points], ncells)
            push!(max_depths[max_points], mdepth)
        end
    end

    numcells_title = "Number of Octree Cells vs Number of Points ($(distname))"
    maxdepths_title = "Maximum Depth vs Number of Points ($(distname))"
    xlabel_ = "Number of Points"
    numcells_ylabel = "Number of Octree Cells"
    maxdepths_ylabel = "Maximum Depth (0-indexed)"
    plt_cells = plot(title=numcells_title, size=(800, 800), legend = :outertopright, xlabel = xlabel_, ylabel = numcells_ylabel)
    plt_depth = plot(title=maxdepths_title, size=(800, 800), legend = :outertopright, xlabel = xlabel_, ylabel = maxdepths_ylabel)
    for mp in max_points_in_file
        cmp_by(i) = num_points[mp][i]
        permutation = sortperm(1:length(num_points[mp]), by=cmp_by)
        num_points[mp] = num_points[mp][permutation]
        num_cells[mp] = num_cells[mp][permutation]
        max_depths[mp] = max_depths[mp][permutation]
        max_point_label = "M = $mp"
        plot!(plt_cells, num_points[mp], num_cells[mp], label = max_point_label, shape = :circle)
        plot!(plt_depth, num_points[mp], max_depths[mp], label = max_point_label, shape = :circle)
    end
    numcells_savefile = "$(distname)_$(dimensions)_num_cells"
    maxdepths_savefile ="$(distname)_$(dimensions)_max_depths"
    savefig(plt_cells, joinpath(outdir, numcells_savefile * ext))
    savefig(plt_depth, joinpath(outdir, maxdepths_savefile * ext))
end

function plot_distribution(directory, distname, dimensions, outdir)
    plotly()
    plot_distribution_helper(directory, distname, dimensions, outdir, ".html")
    gr()
    plot_distribution_helper(directory, distname, dimensions, outdir, ".png")
end
