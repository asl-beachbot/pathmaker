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
          mainCanvas.activate();
          paper.project.activeLayer.removeChildren();
          return api.loadJsonToPaper(data);
        }
      });
    }
  };

  window.loadJsonToPaper = function(data) {
    var c, el, i, path, seg, seg_path, _i, _j, _len, _len1, _ref, _ref1, _results;
    mainCanvas.activate();
    window.filled_segments = [];
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
      if (el.id === 6019205) {
        path.remove();
        path.simplify();
      }
      if (el.type === "POLYGON" || el.type === "FILLED_POLYGON") {
        path.closed = true;
      }
      i = 0;
      if (el.type_int === 1 && el.segments.length) {
        _results.push((function() {
          var _k, _len2, _ref2, _results1;
          _ref2 = el.segments;
          _results1 = [];
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
            _results1.push((function() {
              var _l, _len3, _ref3, _results2;
              _ref3 = seg.coords;
              _results2 = [];
              for (_l = 0, _len3 = _ref3.length; _l < _len3; _l++) {
                c = _ref3[_l];
                _results2.push(seg_path.add(c));
              }
              return _results2;
            })());
          }
          return _results1;
        })());
      } else {
        _results.push(void 0);
      }
    }
    return _results;
  };

}).call(this);
