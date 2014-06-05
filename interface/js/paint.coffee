base_url = "http://localhost:5000/"
window.onload = ->
	hitOptions =
		segments: true
		stroke: true
		curves: true
		handles: true
		fill: true
		tolerance: 10

	paper = window.paper
	canvas = document.getElementById('canvas');
	paper.setup(canvas);
	window.mainCanvas = paper.projects[0];
	populate_paper()
	mainCanvas.activate();
	path = new paper.Path();
	path.strokeColor = 'black';
	paper.view.draw()

	tool = new paper.Tool()
	tool.minDistance = 10
	tool.activate()
	movePath = false
	project = paper.project
	segment = path = oldcenter = handle = null

	tool.onMouseDown = (event) -> 
		prev_selected = paper.project.selectedItems
		paper.project.deselectAll()
		segment = path = null
		hitResult = project.hitTest event.point, hitOptions
		if not hitResult
			return

		hitResult.item.selected = true
		
		console.log(event)
		console.log(hitResult)
		# // if (event.modifiers.shift) {
		# // 	if (hitResult.type == 'segment') {
		# // 		// hitResult.segment.remove();
		# // 	};
		# // 	return;
		# // }

		if hitResult.item.is_segment
			console.log "selected a segment!"
			$("#select_fill").show()

		if (hitResult)
			path = hitResult.item
			if (hitResult.type == 'segment')
				segment = hitResult.segment
				# hitResult.item.fullySelected = true
			else if (hitResult.type == 'stroke') 
				location = hitResult.location;
				# segment = path.insert(location.index + 1, event.point)
			else if (hitResult.type == 'handle-in') 
				handle = hitResult.segment.handleIn
			else if hitResult.type == 'handle-out'
				handle = hitResult.segment.handleOut
			
		movePath = hitResult.type == 'fill'
		if movePath
			project.activeLayer.addChild(hitResult.item);

	tool.onMouseMove = (event) ->
		if event.item
			event.item.selected = true

	tool.onMouseDrag = (event) ->
		if segment
			console.log segment.point
			segment.point = segment.point.add(event.delta);
			console.log segment.point
		else if path
			path.position = path.position.add(event.delta);
		else if handle
			handle.position = handle.position.add(event.delta)
	curr_zoom = 1

	isDragging = false
	prevDragPosition = [0,0]
	$('#canvas').on 'mousedown', (event) ->
		mainCanvas.activate()
		tool.activate()
		$(window).on 'mousemove', ->
			prevDragPosition = [event.pageX, event.pageY]
			isDragging = true
			$(window).off 'mousemove'
	$('#canvas').on 'mouseup', ->
		mainCanvas.activate()
		wasDragging = isDragging
		isDragging = false;
		$(window).off("mousemove")

	$('#canvas').on 'mousemove', (event) ->
		mainCanvas.activate()
		if isDragging and not paper.project.selectedItems.length
			console.log 'dragging'
			delta = [event.pageX - prevDragPosition[0], event.pageY - prevDragPosition[1]]
			prevDragPosition = [event.pageX, event.pageY]
			paper.view.center = paper.view.center.subtract(new paper.Point(delta).multiply(1 / curr_zoom))

	$('#canvas').on 'mousewheel', (event) ->
		mainCanvas.activate()
		console.log(event.deltaX, event.deltaY, event.deltaFactor)
		d = event.deltaY
		zoom_factor = 1
		if d > 0
			zoom_factor = Math.pow(0.8, d)
		else
			zoom_factor = Math.pow(1.2, Math.abs(d))
		beta = 1 / zoom_factor
		curr_zoom = curr_zoom * zoom_factor
		# if not left top, center with parent in mind
		# var parentOffset = $(this).parent().offset(); 
		p = new paper.Point(event.pageX, event.pageY);
		console.log event, p
		pc = p.subtract(paper.view.center)
		a = p.subtract(pc.multiply(beta)).subtract(paper.view.center)
		paper.view.zoom = curr_zoom
		paper.view.center = paper.view.center.add a



	paper.setup(document.getElementById("direction_canvas"))
	directionCanvas = paper.projects[1];
	vectorItem = null
	vector = null
	paintVector = (pos) ->
		directionCanvas.activate()
		vectorStart = new paper.Point(75, 75)
		vectorEnd = pos
		vector =  vectorEnd.subtract(vectorStart);
		if (vectorItem)
			vectorItem.remove();
		items = [];
		arrowVector = vector.normalize(10);
		end = vectorStart + vector;
		vectorItem = new paper.Group([
			new paper.Path([vectorStart, vectorEnd]),
			new paper.Path([
				vectorEnd.add(arrowVector.rotate(135)),
				vectorEnd,
				vectorEnd.add(arrowVector.rotate(-135))
			])
		]);
		vectorItem.strokeColor = 'black';
		vectorItem.strokeWidth = 3
	paintVector(new paper.Point(0,0))
	directionCanvas.activate()
	vectorTool = new paper.Tool()
	vectorTool.activate()
	vectorTool.onMouseDrag = (event) ->
		paintVector(event.point)

	directionCanvas.view.draw()

	$('#testbtn').click( () ->
		dx = 1
		dy = 0
		mainCanvas.activate(	)
		segment = mainCanvas.selectedItems[0];
		if !segment.is_segment 
			return
		if vector
			vector = vector.normalize()
			dx = vector.x
			dy = vector.y
		obj = {
			id: segment._id,
			segment_index: segment.segment_index, 
			dx: dx,
			dy: dy
		}
		$.ajax {
			url: base_url + "update_fill_for_element",
			data: JSON.stringify(obj),
			type: "POST",
			success: (data) ->
				mainCanvas.activate(	)
				paper.project.activeLayer.removeChildren()
				loadJsonToPaper(data)
		}
		# $.post(base_url + "update_fill_for_element", JSON.stringify(obj))
	)
	# code for the direction canvas

