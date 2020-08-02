include("VisualizeImplicitOctree.jl")
using .VisualizeImplicitOctree

using Plots

function visualize(filename, do_animate)
    do_visualize = !do_animate
    all_frames = []
    name = splitext(basename(filename))[1]
    
    if do_visualize
        plotly()
    else
        gr()
    end
    construct_callback = function (objects, properties, framenum)
        if do_animate
            push!(all_frames, save_frame(objects, properties))
        end
        if do_visualize
            visualize_frame(objects, properties, joinpath(pwd(), "..", "out", "$(name)_construct$framenum"))
        end
    end
        
    query_callback = function (objects, properties, querynum)
        if do_animate
          push!(all_frames, save_frame(objects, properties))
        end
        if do_visualize
            visualize_frame(objects, properties, joinpath(pwd(), "..", "out", "$(name)_query$querynum"))
        end
    end
    filetoscene(filename, construct_callback, query_callback)
    if do_animate
        animate_frames(all_frames, joinpath(pwd(), "..", "out", "$(name).gif"))
    end
end

function visualize_script(idir)
    dir = joinpath(pwd(), "..", "data", idir)
    for filename in joinpath.(abspath(dir), readdir(dir))
        if occursin(".octvis", filename)
            println("Visualizing $filename")
            visualize(filename, true)
        end
    end
    for filename in joinpath.(abspath(dir), readdir(dir))
        if occursin(".octvis", filename)
            println("Visualizing $filename")
            visualize(filename, false)
        end
    end
end

function analysis_script(idir)
    plot_distribution(joinpath(pwd(), "..", "distribution_data", idir),
    "normal10000", 2,
    joinpath(pwd(), "..", "distribution_out"))

    plot_distribution(joinpath(pwd(), "..", "distribution_data", idir),
    "poisson100000000", 2,
    joinpath(pwd(), "..", "distribution_out"))

    plot_distribution(joinpath(pwd(), "..", "distribution_data", idir),
    "uniform34641", 2,
    joinpath(pwd(), "..", "distribution_out"))
end

function benchmark_script(idir)
    plot_benchmark(joinpath(pwd(), "..", "benchmark_data", idir),
                   "normal10000", 2,
                   joinpath(pwd(), "..", "benchmark_out"))
    plot_benchmark(joinpath(pwd(), "..", "benchmark_data", idir),
                   "poisson100000000", 2,
                   joinpath(pwd(), "..", "benchmark_out"))
    plot_benchmark(joinpath(pwd(), "..", "benchmark_data", idir),
                   "uniform34641", 2,
                   joinpath(pwd(), "..", "benchmark_out"))
end
 
# visualize_script("")
# analysis_script("")
# benchmark_script("")