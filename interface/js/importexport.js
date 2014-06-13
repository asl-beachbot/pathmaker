// Generated by CoffeeScript 1.7.1
(function() {
  window.mainUrl = 'http://localhost:5000/';

  window.api = {
    resegmentWithLine: function(startPoint, endPoint) {
      var obj;
      obj = {
        x1: startPoint.x,
        y1: startPoint.y,
        x2: endPoint.x,
        y2: endPoint.y
      };
      return $.post(mainUrl + 'resegment_with_line', JSON.stringify(obj), function(data) {
        return loadJsonToPaper(data);
      });
    },
    populatePaper: function() {
      loadJsonToPaper(JSON.parse(window.PolyJSON));
      return $.get("http://localhost:5000/load_svg", function(data) {
        return loadJsonToPaper(data);
      });
    },
    changeFill: function(segment, direction) {
      var obj;
      obj = {
        id: segment._id,
        segment_index: segment.segment_index,
        dx: direction.x,
        dy: direction.y
      };
      return $.ajax({
        url: mainUrl + "update_fill_for_element",
        data: JSON.stringify(obj),
        type: "POST",
        success: function(data) {
          return loadJsonToPaper(data);
        }
      });
    },
    enforceConnection: function(segment1, segment2) {
      var id1, id2, idx1, idx2, obj;
      id1 = segment1.path.id;
      id2 = segment2.path.id;
      idx1 = segment1.index;
      idx2 = segment2.index;
      obj = {
        pair1: segment1.path.id,
        pair2: segment2.path.id,
        pair1_idx: segment1.index,
        pair2_idx: segment2.index
      };
      return $.ajax({
        url: mainUrl + "add_enforced_connection",
        data: JSON.stringify(obj),
        type: "POST",
        success: function(data) {
          return loadJsonToPaper(data);
        }
      });
    }
  };

  window.loadJsonToPaper = function(data) {
    var c, connection_path, el, first, i, path, seg, seg_path, _i, _j, _k, _l, _len, _len1, _len2, _len3, _len4, _m, _ref, _ref1, _ref2, _ref3, _ref4, _results;
    mainCanvas.activate();
    paper.project.activeLayer.removeChildren();
    window.filled_segments = [];
    window.all_connections = [];
    window.painted_elements = [];
    _ref = data.elems;
    _results = [];
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      el = _ref[_i];
      window.currentLoadedData = data;
      path = new paper.Path();
      path.strokeColor = 'black';
      path.strokeWidth = 3;
      path._id = el.id;
      _ref1 = el.coords;
      for (_j = 0, _len1 = _ref1.length; _j < _len1; _j++) {
        c = _ref1[_j];
        path.add(c);
      }
      if (el.type === "POLYGON" || el.type === "FILLED_POLYGON") {
        path.closed = true;
      }
      path.type = el.type;
      painted_elements.push(path);
      i = 0;
      if (el.type_int === 1 && el.segments.length) {
        _ref2 = el.segments;
        for (_k = 0, _len2 = _ref2.length; _k < _len2; _k++) {
          seg = _ref2[_k];
          seg_path = new paper.Path();
          seg_path.strokeColor = "green";
          seg_path.is_segment = true;
          seg_path.fillColor = 'white';
          seg_path.closed = true;
          seg_path._id = el.id;
          seg_path.segment_index = i;
          window.filled_segments.push(seg_path);
          i += 1;
          _ref3 = seg.coords;
          for (_l = 0, _len3 = _ref3.length; _l < _len3; _l++) {
            c = _ref3[_l];
            seg_path.add(c);
          }
        }
      }
      if (el.connection) {
        connection_path = new paper.Path();
        all_connections.push(connection_path);
        connection_path.strokeColor = "blue";
        connection_path.strokeWidth = 3;
        first = true;
        console.log(el.connection);
        _ref4 = el.connection;
        for (_m = 0, _len4 = _ref4.length; _m < _len4; _m++) {
          c = _ref4[_m];
          if (Math.abs(c[1][0]) < 0.00001) {
            continue;
          }
          if (first) {
            connection_path.moveTo(new paper.Point([c[1][0], c[1][1]]));
            first = false;
          } else {
            console.log("CurveTo", c[0][0], c[0][1], c[1][0], c[1][1], c[2][0], c[2][1]);
            connection_path.cubicCurveTo(new paper.Point([c[0][0], c[0][1]]), new paper.Point([c[1][0], c[1][1]]), new paper.Point([c[2][0], c[2][1]]));
          }
        }
        _results.push(connection_path.simplify());
      } else {
        _results.push(void 0);
      }
    }
    return _results;
  };

}).call(this);
