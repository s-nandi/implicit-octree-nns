
function create_plot()
    return plot(legend=false, showaxis=:off, grid=:off, background_color=:white, size=(1000, 1000), caption="hello there")
end

function draw_object!(plt, object::Point2; kwargs...)
    scatter!(plt, [object.x], [object.y]; kwargs...);
end

function draw_object!(plt, object::Line{Point2}; kwargs...)
    plot!(plt, [object.a.x, object.b.x], [object.a.y, object.b.y]; kwargs...);
end

function draw_object!(plt, object::AbstractArray{Point2}; kwargs...)
    plot!(plt, Plots.Shape([(p.x, p.y) for p in object]); kwargs...)
end

function draw_frame(current_plot, objects, props)
    for (object, prop) in zip(objects, props)
        if typeof(object) <: AbstractArray{}
            draw_object!(current_plot, object; prop...)
        end
    end
    for (object, prop) in zip(objects, props)
        if !(typeof(object) <: AbstractArray{})
            draw_object!(current_plot, object; prop...)
        end
    end
end

function save_frame(scene, properties)
    gr()
    current_plot = create_plot()
    draw_frame(current_plot, scene, properties)
    display(current_plot)
    current_plot
end

function animate_frames(plots, ofilename)
    nframes = length(plots)
    anim = @animate for i in 1:nframes
        plot!(plots[i])
    end
    gif(anim, ofilename, fps = 2)
end

function visualize_frame(scene, properties, ofilename)
    plotly()
    current_plot = create_plot()
    println("$(length(scene)) objects")
    draw_frame(current_plot, scene, properties)
    println("Displaying")
    savefig(current_plot, ofilename)
end