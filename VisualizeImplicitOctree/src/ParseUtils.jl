const point_properties = [(:color, :orange), (:markersize, 1.5)]
const box_properties = [(:linecolor, :black), (:fillcolor, :white)]
const ray_properties = [(:color, :brown), (:opacity, 0.5), (:style, :dash)]
const box_midpoint_properties = [(:color, :transparent)]
const voronoi_edge_properties = [(:color, :blue)]
const query_point_properties = [(:color, :magenta), (:markersize, 7.0)]
const query_box_properties = Any[(:opacity, 0.2)]
const too_deep_query_properties = Any[(:color, :red)]
const too_shallow_query_properties = Any[(:color, :orange)]
const just_right_query_properties = Any[(:color, :green)]

function getnumber(::Type{T}, tokens, pos) where {T <: Number}
    @assert pos <= length(tokens)
    parse(T, tokens[pos]), pos + 1
end

function getstring(tokens, pos)
    @assert pos <= length(tokens)
    tokens[pos], pos + 1
end

function getchar(tokens, pos)
    @assert pos <= length(tokens)
    @assert length(tokens[pos]) == 1
    getindex(tokens[pos], 1), pos + 1
end

function addobject(fig, properties, object, props=[])
    push!(fig, object)
    push!(properties, props)
end

function resetframe(current_figure, current_properties)
    empty!(current_figure)
    empty!(current_properties)
end