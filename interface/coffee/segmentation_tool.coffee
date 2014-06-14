# segmentation_tool.coffee
window.segmentation_tool = new paper.Tool()
tool = segmentation_tool
# tool.minDistance = 0

startPoint = endPoint = cutPath = null

tool.onMouseDown = (e) ->
	# console.log e
	if e.event.button != 0
		return
	startPoint = e.point

tool.onMouseDrag = (e) ->
	if e.event.button != 0
		return
	# console.log e
	endPoint = e.point
	if cutPath
		cutPath.remove()
	cutPath = new paper.Path([startPoint, endPoint])
	cutPath.strokeColor = 'red'
	cutPath.strokeWidth = 3

tool.onKeyDown = (e) ->
	if e.key == 'enter' and startPoint and endPoint
		api.resegmentWithLine(startPoint, endPoint)