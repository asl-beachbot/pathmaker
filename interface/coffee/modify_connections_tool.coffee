# modify_connections_tool.coffee
window.modify_connections_tool = new paper.Tool()
tool = modify_connections_tool
hitOptions = 
	segments: true
	tolerance: 20

circles = []
node1 = node2 = null
tool.onMouseDown = (event) ->
	for c in window.painted_elements
		hitResult = c.hitTest(event.point, hitOptions)
		if hitResult
			break
	if hitResult
		console.log hitResult
		hitResult.segment.selected = true
		circle = new paper.Path.Circle(hitResult.segment.point, 10)
		circle.strokeColor = 'magenta'
		circle.strokeWidth = 3
		circles.push circle
		if hitResult.type == 'segment'
			if not node1
				node1 = hitResult.segment.point
			else node2 = hitResult.segment.point
			if node1 and node2
				new_conn_path = new paper.Path(node1.point, node2.point)
				new_conn_path.strokeWidth = 2
				new_conn_path.strokeColor = 'magenta'
				null