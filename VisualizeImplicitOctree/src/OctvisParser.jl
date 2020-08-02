function parsebox(fig, properties, dimensions, tokens, it, custom_properties)
    mins = [0. for _ in 1:dimensions]
    maxs = [0. for _ in 1:dimensions]
    mids = [0. for _ in 1:dimensions]
    for dimension in 1:dimensions
        mins[dimension], it = getnumber(Float64, tokens, it)
        maxs[dimension], it = getnumber(Float64, tokens, it)
        mids[dimension] = (mins[dimension] + maxs[dimension]) / 2 
    end
    if dimensions == 2
        x_min, y_min = mins
        x_max, y_max = maxs
        bottom_left = Point2(x_min, y_min)
        bottom_right = Point2(x_max, y_min)
        top_right = Point2(x_max, y_max)
        top_left = Point2(x_min, y_max)
        addobject(fig, properties, [bottom_left, bottom_right, top_right, top_left], custom_properties)
    elseif dimensions == 3
        @assert false # handle later
    else 
        @assert false
    end
    midpoint = dimensions == 2 ? Point2(mids[1], mids[2]) : Point3(mids[1], mids[2], mids[3])
    midpoint, it
end

function parsepoint(fig, properties, dimensions, tokens, it, custom_properties)
    coords = [0. for _ in 1:dimensions]
    for dimensions in 1:dimensions
        coords[dimensions], it = getnumber(Float64, tokens, it)
    end
    
    if dimensions == 2
        x, y = coords
        addobject(fig, properties, Point2(x, y), custom_properties)
    elseif dimensions == 3
        x, y, z = coords
        addobject(fig, properties, Point3(x, y, z), custom_properties)
    else 
        @assert false
    end
    pt = last(fig)
    pt, it
end

function parseoctreecell(fig, properties, dimensions, tokens, it)
    is_leaf, it = getnumber(Bool, tokens, it)
    box_midpoint, it = parsebox(fig, properties, dimensions, tokens, it, box_properties)
    # addobject(fig, properties, box_midpoint, box_midpoint_properties)
    num_points, it = getnumber(Int64, tokens, it)
    for i in 1:num_points
        point, it = parsepoint(fig, properties, dimensions, tokens, it, point_properties)
        if is_leaf
            addobject(fig, properties, Line(box_midpoint, point), ray_properties)
        end
    end
    it
end

function parsevoronoi(fig, properties, dimensions, tokens, it)
    num_edges, it = getnumber(Int64, tokens, it)
    for i in 1:num_edges
        coords = [0. for i in 1:2dimensions]
        for k in 1:length(coords)
            coords[k], it = getnumber(Float64, tokens, it)
        end
        if dimensions == 2
            x1, y1, x2, y2 = coords
            p1 = Point2(x1, y1)
            p2 = Point2(x2, y2)
            addobject(fig, properties, Line(p1, p2), voronoi_edge_properties)
        elseif dimensions == 3
            x1, y1, z1, x2, y2, z2 = coords
            p1 = Point3(x1, y1, z1)
            p2 = Point3(x2, y2, z3)
            addobject(fig, properties, Line(p1, p2), voronoi_edge_properties)
        else
            @assert false
        end
    end
    it
end

function parsequerystep(fig, properties, dimensions, tokens, it)
    pt, it = parsepoint(fig, properties, dimensions, tokens, it, query_point_properties)
    depth, it = getnumber(Int32, tokens, it)
    status, it = getstring(tokens, it)
    curr_query_box_properties = copy(query_box_properties)
    extra_query_box_properties = []
    if status == "too_deep"
        extra_query_box_properties = too_deep_query_properties
    elseif status == "too_shallow"
        extra_query_box_properties = too_shallow_query_properties
    elseif status == "just_right"
        extra_query_box_properties = just_right_query_properties
    else
        println("Got incorrect query status $status")
        @assert false
    end
    append!(curr_query_box_properties, extra_query_box_properties)
    query_box_midpoint, it = parsebox(fig, properties, dimensions, tokens, it, curr_query_box_properties)
    it
end


function parsetokens(dimensions, tokens)
    println("Parsing frame")
    it = 1
    current_figure = []
    current_properties = []
    while it <= length(tokens)
        token_type, it = getstring(tokens, it)
        if token_type == "octree_cell"
            it = parseoctreecell(current_figure, current_properties, dimensions, tokens, it)
        elseif token_type == "voronoi_edges"
            it = parsevoronoi(current_figure, current_properties, dimensions, tokens, it)
        elseif token_type == "query_point"
            it = parsequerystep(current_figure, current_properties, dimensions, tokens, it)
        elseif token_type == "end_frame" 
            break
        elseif token_type == "end_query"
            break
        else 
            println("Bad token:", token_type)
            @assert false
        end
    end
    current_figure, current_properties
end

function parseio(io, construct_callback, query_callback)
    tokens = []
    dimensions = 0
    framenum = 1
    querynum = 1
    final_objects = []
    final_properties = []
    for (line_number, line) in enumerate(eachline(io))
        if line_number == 1
            dimensions, it = getnumber(Int32, split(line)[1], 1)
            println("Dimensions: $dimensions")
        else
            for token in split(line)
                push!(tokens, token)
                if token == "end_frame"
                    @assert dimensions in [2, 3]
                    objects, properties = parsetokens(dimensions, tokens)
                    final_objects = copy(objects)
                    final_properties = copy(properties)
                    println("Frame $framenum")
                    construct_callback(objects, properties, framenum)
                    framenum += 1
                    empty!(tokens)
                elseif token == "end_query"
                    @assert dimensions in [2, 3]
                    new_objects, new_properties = parsetokens(dimensions, tokens)
                    objects = copy(final_objects)
                    properties = copy(final_properties)
                    append!(objects, new_objects)
                    append!(properties, new_properties)
                    println("Query $querynum")
                    query_callback(objects, properties, querynum)
                    querynum += 1
                    empty!(tokens)
                end
            end
        end
    end
end

function filetoscene(filename, construct_callback, query_callback)
    open(filename, "r") do io
        parseio(io, construct_callback, query_callback)
    end
end