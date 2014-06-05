# Define adapter interface for talking to server
# parsed_json = JSON.parse(window.PolyJSON)

window.loadJsonToPaper = (data) ->
	mainCanvas.activate()
	for el in data.elems
		window.currentLoadedData = data
		path = new paper.Path()
		path.strokeColor = 'black';
		path.strokeWidth = 3
		path._id = el.id
		for c in el.coords
			path.add(c)
		if el.id == 6019205
			path.remove()
			path.simplify()
		if el.type == "POLYGON" or el.type == "FILLED_POLYGON"
			path.closed = true;
		# path.fillColor = new paper.Color(1,0,0)
		i = 0
		if el.type_int == 1 and el.segments.length
			for seg in el.segments
				seg_path = new paper.Path()
				seg_path.strokeColor = "green"
				seg_path.is_segment = true
				seg_path.closed = true
				seg_path._id = el.id
				seg_path.segment_index = i
				i += 1
				for c in seg.coords
					seg_path.add(c)

window.populate_paper = () ->
	# loadJsonToPaper(JSON.parse(window.PolyJSON))
	$.get "http://localhost:5000/load_svg", (data) ->
		loadJsonToPaper(data);