function parse_benchmark_file(filename)
    time_elapsed = 0
    open(filename, "r") do io
        time_elapsed = parse(Int64, split(collect(eachline(io))[1])[1])
    end
    time_elapsed
end

function plot_benchmark_helper(directory, distname, dimensions, is_construct, outdir, ext)
    num_points = Dict()
    times = Dict()
    labels_in_file = Set()
    for filename in readdir(directory)
        if occursin(distname, filename)
            tokens = split(filename, "_")
            curr_dimensions = parse(Int32, tokens[2])
            npoints = parse(Int64, tokens[3])
            label = tokens[6]

            if curr_dimensions != dimensions
                continue
            end
            if is_construct && !occursin("construct", filename)
                continue
            end
            if !is_construct && !occursin("query", filename)
                continue
            end

            timetaken = parse_benchmark_file(joinpath(directory, filename))
            if label ∉ labels_in_file
                push!(labels_in_file, label)
                num_points[label] = []
                times[label] = []
            end
            push!(num_points[label], npoints)
            push!(times[label], timetaken)
        end
    end
    construction_title = "Construction Time vs Number of Points ($(distname))"
    query_title = "Time per Query vs Number of Points ($(distname))"
    title_ = is_construct ? construction_title : query_title
    xlabel_ = "Number of Points"
    construction_ylabel = "Construction Time (ns)"
    query_ylabel = "Time per Query (ns)"
    ylabel_ = is_construct ? construction_ylabel : query_ylabel
    plt = plot(title=title_, size=(800, 800), legend=:outertopright, 
                xlabel=xlabel_, ylabel=ylabel_)
    for lab in labels_in_file
        cmp_by(i) = num_points[lab][i]
        permutation = sortperm(1:length(num_points[lab]), by=cmp_by)
        num_points[lab] = num_points[lab][permutation]
        times[lab] = times[lab][permutation]
        locator_label = tryparse(Float64, lab) != nothing ? "M = $lab" : lab
        plot!(plt, num_points[lab], times[lab], label=locator_label, shape=:circle)
    end
    construction_savefile = "$(distname)_$(dimensions)_construction"
    query_savefile = "$(distname)_$(dimensions)_queries"
    savefile = is_construct ? construction_savefile : query_savefile
    savefig(plt, joinpath(outdir, savefile * ext))
end

function plot_benchmark(directory, distname, dimensions, outdir)
    plotly()
    plot_benchmark_helper(directory, distname, dimensions, true, outdir, ".html")
    plot_benchmark_helper(directory, distname, dimensions, false, outdir, ".html")
    gr()
    plot_benchmark_helper(directory, distname, dimensions, true, outdir, ".png")
    plot_benchmark_helper(directory, distname, dimensions, false, outdir, ".png")
end