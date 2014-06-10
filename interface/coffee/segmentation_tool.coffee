# segmentation_tool.coffee
window.segmentation_tool = new paper.Tool()
tool = segmentation_tool
tool.minDistance = 0

startPoint = endPoint = cutPath = null

tool.onMouseDown = (e) ->
	console.log e
	startPoint = e.point
tool.onMouseMove = (e) ->
	console.log e
tool.onMouseDrag = (e) ->
	console.log e
	endPoint = e.point
	if cutPath
		cutPath.remove()
	cutPath = new paper.Path([startPoint, endPoint])
	cutPath.strokeColor = 'red'
	cutPath.strokeWidth = 3
tool.onKeyDown = (e) ->
	console.log e
	if e.key == 'enter' and startPoint and endPoint
		api.resegmentWithLine(startPoint, endPoint)