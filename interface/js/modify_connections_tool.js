// Generated by CoffeeScript 1.7.1
(function() {
  var circles, hitOptions, new_conn_path, node1, node2, tool;

  window.modify_connections_tool = new paper.Tool();

  tool = modify_connections_tool;

  hitOptions = {
    segments: true,
    tolerance: 20
  };

  circles = [];

  new_conn_path = null;

  node1 = node2 = null;

  tool.onMouseDown = function(event) {
    var c, circle, hitResult, _i, _len, _ref;
    _ref = window.painted_elements;
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      c = _ref[_i];
      hitResult = c.hitTest(event.point, hitOptions);
      if (hitResult) {
        break;
      }
    }
    if (hitResult) {
      console.log(hitResult);
      hitResult.segment.selected = true;
      circle = new paper.Path.Circle(hitResult.segment.point, 10);
      circle.strokeColor = 'magenta';
      circle.strokeWidth = 3;
      circles.push(circle);
      if (hitResult.type === 'segment') {
        if (!node1) {
          node1 = hitResult.segment;
        } else {
          node2 = hitResult.segment;
        }
        if (node1 && node2) {
          console.log(node1);
          console.log("paointing path");
          new_conn_path = new paper.Path([[node1.point.x, node1.point.y], [node2.point.x, node2.point.y]]);
          new_conn_path.strokeWidth = 2;
          new_conn_path.strokeColor = 'magenta';
          api.enforceConnection(node1, node2);
          return node1 = node2 = null;
        }
      }
    }
  };

}).call(this);
