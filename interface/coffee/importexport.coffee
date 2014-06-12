# Define adapter interface for talking to server
# parsed_json = JSON.parse(window.PolyJSON)

window.mainUrl = 'http://localhost:5000/'

window.api =
	resegmentWithLine: (startPoint, endPoint) ->
		obj = 
			x1: startPoint.x
			y1: startPoint.y
			x2: endPoint.x
			y2: endPoint.y

		$.post mainUrl + 'resegment_with_line', JSON.stringify(obj), (data) ->
			loadJsonToPaper(data)

	populatePaper: () ->
		# loadJsonToPaper(JSON.parse(window.PolyJSON))
		$.get "http://localhost:5000/load_svg", (data) ->
			loadJsonToPaper(data);

	changeFill: (segment, direction) ->
		obj =
			id: segment._id
			segment_index: segment.segment_index 
			dx: direction.x
			dy: direction.y

		$.ajax
			url: mainUrl + "update_fill_for_element",
			data: JSON.stringify(obj),
			type: "POST",
			success: (data) ->
				api.loadJsonToPaper(data)


window.loadJsonToPaper = (data) ->
	mainCanvas.activate()
	paper.project.activeLayer.removeChildren()
	window.filled_segments = []
	window.all_connections = []
	for el in data.elems
		window.currentLoadedData = data
		path = new paper.Path()
		path.strokeColor = 'black';
		path.strokeWidth = 3
		path._id = el.id
		for c in el.coords
			path.add(c)
		if el.type == "POLYGON" or el.type == "FILLED_POLYGON"
			path.closed = true;
		# path.fillColor = new paper.Color(1,0,0)
		i = 0
		if el.type_int == 1 and el.segments.length
			for seg in el.segments
				seg_path = new paper.Path()
				seg_path.strokeColor = "green"
				seg_path.is_segment = true
				seg_path.fillColor = 'white'
				seg_path.closed = true
				seg_path._id = el.id
				seg_path.segment_index = i
				window.filled_segments.push(seg_path);
				i += 1
				for c in seg.coords
					seg_path.add(c)
		if el.connection
			connection_path = new paper.Path()
			all_connections.push(connection_path)
			connection_path.strokeColor = "blue"
			connection_path.strokeWidth = 3
			first = true
			console.log el.connection
			for c in el.connection
				if Math.abs(c[1][0]) < 0.00001
					continue

				if first
					connection_path.moveTo(new paper.Point([c[1][0], c[1][1]]))
					first = false
				else
					console.log "CurveTo", c[0][0], c[0][1], c[1][0], c[1][1], c[2][0], c[2][1]
					connection_path.cubicCurveTo(
						new paper.Point([c[0][0], c[0][1]])
						new paper.Point([c[1][0], c[1][1]])
						new paper.Point([c[2][0], c[2][1]])
					)
			connection_path.simplify()
