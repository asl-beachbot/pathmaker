// Generated by CoffeeScript 1.7.1
(function() {
  var cutPath, endPoint, startPoint, tool;

  window.segmentation_tool = new paper.Tool();

  tool = segmentation_tool;

  tool.minDistance = 0;

  startPoint = endPoint = cutPath = null;

  tool.onMouseDown = function(e) {
    console.log(e);
    return startPoint = e.point;
  };

  tool.onMouseMove = function(e) {
    return console.log(e);
  };

  tool.onMouseDrag = function(e) {
    console.log(e);
    endPoint = e.point;
    if (cutPath) {
      cutPath.remove();
    }
    cutPath = new paper.Path([startPoint, endPoint]);
    cutPath.strokeColor = 'red';
    return cutPath.strokeWidth = 3;
  };

  tool.onKeyDown = function(e) {
    console.log(e);
    if (e.key === 'enter' && startPoint && endPoint) {
      return api.resegmentWithLine(startPoint, endPoint);
    }
  };

}).call(this);