import json
import random
import threading
import math
import sys
sys.path.append("/home/beachbot/Programs/pathmaker")
sys.path.append("/home/beachbot/Programs/pathmaker/python")

from flask import Flask, app, render_template, make_response, request

from beachbot_pathgen import Generator
generator = Generator()

app = Flask(__name__)
@app.route("/hallo")
def hallo():
    return "Hallo!"

@app.route('/load_svg')
def load_svg():
    generator.parse_file("./assets/fill_test.svg")
    j = generator.run_routine()
    resp = make_response(j, 200)
    resp.headers['Content-Type'] = "application/json"
    resp.headers['Access-Control-Allow-Origin'] = "*"
    return resp

@app.route('/update_fill_for_element', methods=['POST', 'GET'])
def update_fill_for_element():
    print("update fill!")
    j = request.get_data()
    print(j)
    return_j = generator.update_fill_for_element(j)

    resp = make_response(return_j, 200)
    resp.headers['Content-Type'] = "application/json"
    resp.headers['Access-Control-Allow-Origin'] = "*"
    return resp

@app.route('/resegment_with_line', methods=['POST'])
def resegment_with_line():
    print("new segmentation")
    j = request.get_data()
    return_j = generator.resegment_with_line(j);
    resp = make_response(return_j, 200)
    resp.headers['Content-Type'] = "application/json"
    resp.headers['Access-Control-Allow-Origin'] = "*"
    return resp

@app.route('/add_enforced_connection', methods=['POST'])
def add_enforced_connection():
    print("enforcing a new connection")
    j = request.get_data()
    return_j = generator.add_enforced_connection(j);
    resp = make_response(return_j, 200)
    resp.headers['Content-Type'] = "application/json"
    resp.headers['Access-Control-Allow-Origin'] = "*"
    return resp

if __name__ == '__main__':
    app.run(debug=False)