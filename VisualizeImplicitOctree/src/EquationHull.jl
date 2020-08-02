using Plots

gr()
Point = Array{Float32,2}

struct equation 
    m::Float32
    b::Float32
end

function evaluate_at(e, x) 
    e.m * x + e.b
end

function intersection_with(e1, e2)
    @assert e1.m != e2.m 
    insct = (e2.b - e1.b) / (e1.m - e2.m)
    insct
end

function draw_line!(plt, x1, y1, x2, y2, ith)
    plot!(plt, [x1, x2], [y1, y2], color=ith)
end

function draw_dash_line!(plt, x1, y1, x2, y2, ith)
    plot!(plt, [x1, x2], [y1, y2], color=ith, style=:dash, opacity=0.5)
end

function draw_point!(plt, x, y, ith)
    scatter!(plt, [x], [y], color=ith)
end

function draw_state(points, hull, inds, y, xlo, xhi, ylo, yhi, highlight=[], lighter=[])
    point_space = plot(legend=false, ylims=[ylo, yhi], xlims=[xlo, xhi], xlabel = "x", ylabel = "y",
                       title = "Point Space")
    distance_space = plot(legend=false, xlims=[xlo, xhi], xlabel = "x", ylabel = "dist(x)", title = "Distance Space")
    
    draw_line!(point_space, xlo, y, xhi, y, -1)
    for i in inds
        draw_point!(point_space, points[i][1], points[i][2], i in highlight ? :gray : i)

    end

    for (it, (i, h)) in enumerate(zip(inds, hull))
        x_prv = it == 1 ? xlo : intersection_with(hull[it - 1], h)
        x_nxt = it == size(hull, 1) ? xhi : intersection_with(h, hull[it + 1])
        y_prv = evaluate_at(h, x_prv)
        y_nxt = evaluate_at(h, x_nxt)

        distance_function(x) = (y - points[i][2])^2 + (x - points[i][1])^2
        if i in highlight
            plot!(distance_space, distance_function, xlo, xhi, color=:gray, markershape=:x)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_prv, y, :gray)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_nxt, y, :gray)
        elseif i in lighter
            plot!(distance_space, distance_function, xlo, xhi, color=i, alpha=0.3)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_prv, y, i)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_nxt, y, i)
        else
            plot!(distance_space, distance_function, xlo, xhi, color=i)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_prv, y, i)
            draw_dash_line!(point_space, points[i][1], points[i][2], x_nxt, y, i)
        end
        # draw_line!(distance_space, x_prv, y_prv, x_nxt, y_nxt, inds[i])    
    end
    both = plot(point_space, distance_space, layout=(2, 1), legend=false, size=(1000, 1000))
    display(both)
    both
end


function intersect_before(a, b, c)
    intersection_with(a, b) <= intersection_with(b, c)
end

function split_demo(points, y)
    min_y = minimum(pt -> pt[2], points)
    max_y = maximum(pt -> pt[2], points)
    min_x = minimum(pt -> pt[1], points)
    max_x = maximum(pt -> pt[1], points)

    toequation = function (point)
        equation(-2point[1], point[1]^2 + (point[2] - y)^2)
    end
    equations = map(toequation, points)

    perm = sortperm(equations, by=(f(eq) = eq.m), rev=true)
    equations = equations[perm]
    points = points[perm]


    hull = Vector{equation}()
    inds = Vector{Int}()

    allplots = []
    for (i, e) in enumerate(equations)
        push!(hull, e)
        push!(inds, i)
        plt = draw_state(points, hull, inds, y, min_x, max_x, min_y, max_y, [], [i])
        push!(allplots, plt)
        while size(hull, 1) >= 3 && intersect_before(hull[end], hull[end - 2], hull[end - 1])
            plt = draw_state(points, hull, inds, y, min_x, max_x, min_y, max_y, [inds[end - 1]])
            push!(allplots, plt)
            deleteat!(hull, size(hull, 1) - 1)
            deleteat!(inds, size(inds, 1) - 1)
        end
        plt = draw_state(points, hull, inds, y, min_x, max_x, min_y, max_y)
        push!(allplots, plt);
    end
    allplots
end

plots = split_demo([[-1, -3], [0, -3], [2, 3], [4, 1], [8, -4], [10, 0.1]], 0)
nframes = length(plots)
anim = @animate for i in 1:nframes
    plot!(plots[i])
end
gif(anim, "split_line_demo.gif", fps=1)
