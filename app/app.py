import os
from flask import Flask, render_template, Blueprint
from src.balanca import balanca_blueprint
from src.subscriber import *
app = Flask(__name__)
app.register_blueprint(balanca_blueprint)

if __name__ == '__main__':
    app.run(debug=True)
