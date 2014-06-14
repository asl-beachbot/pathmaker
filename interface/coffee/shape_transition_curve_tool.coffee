# shape_transition_curve_tool.coffee

window.shape_transition_curve_tool = new paper.Tool()


tool = shape_transition_curve_tool
# tool.minDistance = 2
handle = handle2 = node = null
hitOptions =
	segments: true
	stroke: true
	handles: true
	tolerance: 5

# bezier_Path = new paper.Path()
# bezier_Path.add([10, 10])
# bezier_Path.add(new paper.Segment(new paper.Point(100, 200), new paper.Point(-30, -40), new paper.Point(40, 50)))
# bezier_Path.add([300, 400])
# bezier_Path.strokeColor = 'green'

# bezier_Path.strokeWidth = 3

# bezier_Path.fullySelected = true
# test if last: 
# bezier_Path.segments[bezierPath.segments.length] === hitResult.segment 
curr_seg = null
tool.onMouseDown = (event) ->
	if event.event.button != 0
		return
	node = handle = handle2 = null
	# console.log event
	for c in window.all_connections
		hitResult = c.hitTest(event.point, hitOptions)
		if hitResult
			break
	if not hitResult
		paper.project.deselectAll()
	if hitResult
		console.log hitResult
		curr_seg = hitResult.segment
		hitResult.item.fullySelected = true
		if hitResult.type == 'handle-in'
			handle2 = hitResult.segment.handleOut
			handle = hitResult.segment.handleIn
		else if hitResult.type == 'handle-out'
			handle2 = hitResult.segment.handleIn
			handle = hitResult.segment.handleOut
		else if hitResult.type == 'segment'
			node = hitResult.segment.point
		# else if (hitResult.type == 'stroke') 
		# 	location = hitResult.location;
		# 	segment = hitResult.item.insert(location.index + 1, event.point)
	return true

tool.onMouseDrag = (event) ->
	console.log event
	console.log node
	if event.event.button != 0
		return
	if (handle)
		handle.x += event.delta.x
		handle.y += event.delta.y
		c_p = curr_seg.point
		v_norm = handle
		v_norm = v_norm.divide(v_norm.length)
		l_2 = (handle2).length
		handle2.x = (v_norm.multiply(-1).multiply(l_2)).x
		handle2.y = (v_norm.multiply(-1).multiply(l_2)).y
		paper.view.update()
	else if node
		node.x += event.delta.x
		node.y += event.delta.y
		paper.view.update()

	return true