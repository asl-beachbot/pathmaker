# shape_transition_curve_tool.coffee

window.shape_transition_curve_tool = new paper.Tool()


tool = shape_transition_curve_tool
tool.minDistance = 0
handle = handle2 = node = null
hitOptions =
	segments: true
	stroke: true
	handles: true
	tolerance: 10

# bezier_Path = new paper.Path()
# bezier_Path.add([10, 10])
# bezier_Path.add(new paper.Segment(new paper.Point(100, 200), new paper.Point(-30, -40), new paper.Point(40, 50)))
# bezier_Path.add([300, 400])
# bezier_Path.strokeColor = 'green'

# bezier_Path.strokeWidth = 3

# bezier_Path.fullySelected = true
# test if last: 
# bezier_Path.segments[bezierPath.segments.length] === hitResult.segment 
tool.onMouseDown = (event) ->
	node = handle = handle2 = null
	console.log event
	for c in window.all_connections
		hitResult = c.hitTest(event.point, hitOptions)
		if hitResult
			break
	if hitResult
		console.log hitResult
		hitResult.item.fullySelected = true
		if hitResult.type == 'handle-in'
			handle2 = hitResult.segment.handleOut
			handle = hitResult.segment.handleIn
		else if hitResult.type == 'handle-out'
			handle2 = hitResult.segment.handleIn
			handle = hitResult.segment.handleOut
		else if hitResult.type == 'segment'
			node = hitResult.segment.point
		else if (hitResult.type == 'stroke') 
			location = hitResult.location;
			segment = path.insert(location.index + 1, event.point)
tool.onMouseDrag = (event) ->
	if (handle)
		handle.x += event.delta.x
		handle.y += event.delta.y
		handle2.x -= event.delta.x
		handle2.y -= event.delta.y
	else if node
		node.x += event.delta.x
		node.y += event.delta.y
	