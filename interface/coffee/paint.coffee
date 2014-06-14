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

window.pp_opts = 
	outline: true

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
	$('#canvas').on 'mousedown', (event) ->
		if event.which != 3
			return
		mainCanvas.activate()
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
		if isDragging
			mainCanvas.activate()
			delta = [event.pageX - prevDragPosition[0], event.pageY - prevDragPosition[1]]
			prevDragPosition = [event.pageX, event.pageY]
			paper.view.center = paper.view.center.subtract(new paper.Point(delta))

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
			p = new paper.Point(event.pageX, -event.pageY);
			p.multiply(zoom_factor)
			pc = p.subtract(paper.view.center)
			a = p.subtract(pc.multiply(beta)).subtract(paper.view.center)
			paper.view.zoom = curr_zoom
			paper.view.center = paper.view.center.add a

	$('#resegment').click () ->
		segmentation_tool.activate()
	$('#fill').click () ->
		fill_select_tool.activate()
		$('#fill_options').show()
	$('#connections').click () ->
		modify_connections_tool.activate()
	$('#shape_transitions').click () ->
		console.log "activating the shape transition tool"
		shape_transition_curve_tool.activate()

	window.changeOutline = () ->
		if window.pp_opts.outline
			# for w in window.painted_elements
			# 	w.strokeWidth = w.origStrokeWidth
			# 	console.log w.origStrokeWidth

			for el in paper.project.getItems({class: paper.Path})
				el.strokeWidth = 1
		else 
			# for w in window.painted_elements
			# 	w.strokeWidth = 1

			for el in paper.project.getItems({class: paper.Path})
				el.strokeWidth = el.origStrokeWidth
		paper.view.update()

	window.changeDispConn = () ->
		if window.pp_opts.show_conn == 0
			for c in all_connections
				c.visible = false
			for c in window.simple_all_connections
				c.visible = false
		else if window.pp_opts.show_conn == 1
			for c in all_connections
				c.visible = true
			for c in window.simple_all_connections
				c.visible = false
		else if window.pp_opts.show_conn == 2
			for c in window.simple_all_connections
				c.visible = true
			for c in all_connections
				c.visible = false
		paper.view.update
				# simple_conns.push(simple_c)

	$('#outline').on 'change', () ->
		if $(this).is(':checked')
			window.pp_opts.outline = true
			changeOutline()

	$('#real').on 'change', () ->
		if $(this).is(':checked')
			window.pp_opts.outline = false
			changeOutline()


	$('#hide_conn').on 'change', () ->
		if $(this).is(':checked')
			window.pp_opts.show_conn = 0
			changeDispConn()

	$('#show_conn').on 'change', () ->
		if $(this).is(':checked')
			window.pp_opts.show_conn = 1
			changeDispConn()

	$('#simple_conn').on 'change', () ->
		if $(this).is(':checked')
			window.pp_opts.show_conn = 2
			changeDispConn()

