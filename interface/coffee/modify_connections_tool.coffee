# modify_connections_tool.coffee
window.modify_connections_tool = new paper.Tool()
tool = modify_connections_tool
hitOptions = 
	segments: true
	tolerance: 20

circles = []
new_conn_path = null;
node1 = node2 = null

getNode = (seg) ->
	p = seg.path
	node = null
	if not p.closed
		if seg in [p.firstSegment, p.lastSegment]
			node = seg
		else
			i = seg.index
			console.log i, p.segments.length
			if p.segments.length - i < i
				node = p.lastSegment
			else 
				node = p.firstSegment
	else
		node = seg
	return node

tool.onMouseDown = (event) ->
	if event.event.button != 0
		return
	for c in window.painted_elements
		hitResult = c.hitTest(event.point, hitOptions)
		console.log hitResult
		if hitResult
			break
	if hitResult
		if hitResult.type == 'segment'
			if not node1
				node1 = getNode(hitResult.segment)
				selected_node = node1
			else 
				node2 = getNode(hitResult.segment)
				selected_node = node2
			if node1 and node2
				console.log(node1)
				console.log "paointing path"
				new_conn_path = new paper.Path([[node1.point.x, node1.point.y], [node2.point.x, node2.point.y]])
				new_conn_path.strokeWidth = 2
				new_conn_path.strokeColor = 'magenta'
				api.enforceConnection(node1, node2)
				node1 = node2 = null
		console.log hitResult
		selected_node.selected = true
		circle = new paper.Path.Circle(selected_node.point, 10)
		circle.strokeColor = 'magenta'
		circle.strokeWidth = 3
		circles.push circle
tool.onKeyDown = (event) ->
	console.log event