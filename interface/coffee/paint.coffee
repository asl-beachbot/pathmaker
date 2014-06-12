# What Tools are needed?
#
# 1. Segmentation Tool
#    Draw a line which segments the polygon thats
#    below the lines along the intersection points
#    Maybe interpolate intersection points and possibly
#    merge with Polygon points
#
# 2. Fill - Direction Tool
#    Draw an Arrow inside a convex segment for filling the
#    Segment with the wiggle fill in the specific direction
#    This tool can also select a segment. After a segment is 
#    selected, either fill direction can be choosen or spiral 
#    fill
#
# 3. Shape transition curves tool
#    This tool can shape the transition curves
#
# 4. Manual connection tool
#    Wherever the TSP doesn't find a good solution, the MCT
#    should allow the user to connect lines and polygons at 
#    arbitrary points

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
	api.populatePaper()
	mainCanvas.activate();
	paper.view.draw()

	segmentation_tool.activate()
	# shape_transition_curve_tool.init()
	# shape_transition_curve_tool.activate()
	# fill_select_tool.activate()

	# canvas move and zoom functions
	curr_zoom = 1
	isDragging = false
	prevDragPosition = [0,0]
	# $('#canvas').on 'mousedown', (event) ->
	# 	# if(event.which != 2) {return;}
	# 	mainCanvas.activate()
	# 	$(window).on 'mousemove', ->
	# 		prevDragPosition = [event.pageX, event.pageY]
	# 		isDragging = true
	# 		$(window).off 'mousemove'

	# $('#canvas').on 'mouseup', ->
	# 	mainCanvas.activate()
	# 	wasDragging = isDragging
	# 	isDragging = false;
	# 	$(window).off("mousemove")

	# $('#canvas').on 'mousemove', (event) ->
	# 	mainCanvas.activate()
	# 	if isDragging and not paper.project.selectedItems.length
	# 		console.log 'dragging'
	# 		delta = [event.pageX - prevDragPosition[0], event.pageY - prevDragPosition[1]]
	# 		prevDragPosition = [event.pageX, event.pageY]
	# 		paper.view.center = paper.view.center.subtract(new paper.Point(delta).multiply(1 / curr_zoom))

	$('#canvas').on 'mousewheel', (event) ->
		mainCanvas.activate()
		if event.altKey
			paper.view.center = paper.view.center.add new paper.Point(event.deltaX * 10, event.deltaY * 10)
		else
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
			pc = p.subtract(paper.view.center)
			a = p.subtract(pc.multiply(beta)).subtract(paper.view.center)
			paper.view.zoom = curr_zoom
			paper.view.center = paper.view.center.add a

	# paper.setup(document.getElementById("direction_canvas"))
	# directionCanvas = paper.projects[1];
	# vectorItem = null
	# vector = null
	# paintVector = (pos) ->
	# 	directionCanvas.activate()
	# 	vectorStart = new paper.Point(75, 75)
	# 	vectorEnd = pos
	# 	vector =  vectorEnd.subtract(vectorStart);
	# 	if (vectorItem)
	# 		vectorItem.remove();
	# 	items = [];
	# 	arrowVector = vector.normalize(10);
	# 	end = vectorStart + vector;
	# 	vectorItem = new paper.Group([
	# 		new paper.Path([vectorStart, vectorEnd]),
	# 		new paper.Path([
	# 			vectorEnd.add(arrowVector.rotate(135)),
	# 			vectorEnd,
	# 			vectorEnd.add(arrowVector.rotate(-135))
	# 		])
	# 	]);
	# 	vectorItem.strokeColor = 'black';
	# 	vectorItem.strokeWidth = 3
	# paintVector(new paper.Point(0,0))
	# directionCanvas.activate()
	# vectorTool = new paper.Tool()
	# vectorTool.activate()
	# vectorTool.onMouseDrag = (event) ->
	# 	paintVector(event.point)

	# directionCanvas.view.draw()

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
		changeFill(segment, vector)
	)
	# <button id="resegment">Segmentation Tool</button>
	# <button id="fill">Select Fill Tool</button>
	# <button id="connections">Change Connections</button>
	# <button id="shape_transitions">Modify Transitions</button>

	$('#resegment').click () ->
		segmentation_tool.activate()
	$('#fill').click () ->
		fill_select_tool.activate()
	$('#connections').click () ->
		modify_connections_tool.activate()
	$('#shape_transitions').click () ->
		console.log "activating the shape transition tool"
		shape_transition_curve_tool.activate()


	# code for the direction canvas

