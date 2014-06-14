# fill_select_tool.coffee
window.fill_select_tool = new paper.Tool()
tool = fill_select_tool;
tool.minDistance = 1
tool.activate()
movePath = false
project = paper.project
segment = path = oldcenter = handle = null
hitOptions = 
	fill: true
	segments: true
	tolerance: 10
	stroke: true

startVector = endVector = vector = null
$(document).ready () ->
	$('#spiral_fill').click () ->
		console.log 'click spirals'
		if segment
			p = new paper.Point(0, 1)
			api.changeFill(segment, p, 2)
	return

tool.onMouseDown = (event) -> 
	if event.event.button != 0
		return

	prev_selected = paper.project.selectedItems
	paper.project.deselectAll()
	# todo only check segments and polys
	if movePath
		startVector = event.point
	for seg in filled_segments
		hitResult = seg.hitTest event.point, hitOptions
		if hitResult
			break
	if not hitResult
		startVector = event.point
		return
	hitResult.item.selected = true
	
	# // if (event.modifiers.shift) {
	# // 	if (hitResult.type == 'segment') {
	# // 		// hitResult.segment.remove();
	# // 	};
	# // 	return;
	# // }

	if hitResult.item.is_segment
		console.log "selected a segment!"
		$("#select_fill").show()

	if hitResult
		segment = path = hitResult.item
		# if (hitResult.type == 'segment')
		# 	segment = hitResult.segment
			# hitResult.item.fullySelected = true
	else if (hitResult.type == 'stroke')
		console.log(hitResult)
		location = hitResult.location;
		segment = hitResult.item
		# segment = path.insert(location.index + 1, event.point)
		# else if (hitResult.type == 'handle-in') 
		# 	handle = hitResult.segment.handleIn
		# else if hitResult.type == 'handle-out'
		# 	handle = hitResult.segment.handleOut
		
	movePath = hitResult.type == 'fill'
	# if movePath
	# 	project.activeLayer.addChild(hitResult.item);

# tool.onMouseMove = (event) ->
# 	if event.item
# 		event.item.selected = true

tool.onMouseDrag = (event) ->
	if event.event.button != 0
		return

	if startVector
		endVector = event.point
		if vector
			vector.remove()
		vector = new paper.Path(startVector, endVector)
		vector.strokeColor = 'red'
		vector.strokeWidth = 4
		return
	if segment
		console.log segment.point
		segment.point = segment.point.add(event.delta);
		console.log segment.point
	else if path
		path.position = path.position.add(event.delta);
	else if handle
		handle.position = handle.position.add(event.delta)
curr_zoom = 1

tool.onMouseUp = (event) ->
	if event.event.button != 0
		return

	if segment and vector
		p = (startVector.subtract(endVector)).divide((startVector.subtract(endVector)).length)
		console.log(p)
		api.changeFill(segment, p)
