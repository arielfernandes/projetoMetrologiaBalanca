from flask import Blueprint, redirect, render_template, request
from src.subscriber import Mqtt
import time
balanca_blueprint = Blueprint('balanca', '__name__', template_folder='templates')

@balanca_blueprint.route('/')
def index():
    return render_template('index.html')

@balanca_blueprint.route('/mqtt')
def mqtt(name=None):
    mqtt = Mqtt()
    try:
      name = mqtt.connect_mqtt()
    except:
        name = 'Connection error'
    
    name = f'{name} Kg'
   
    return render_template('index.html', name=name)
    
