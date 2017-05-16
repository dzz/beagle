import os
import importlib
import sys

def get_app_from_path(app_directory, module ):
    sys.path.append(app_directory)
    container_module = importlib.import_module( module )
    main_module = importlib.import_module( ".main", module )
    return main_module

def get_app(app_name):
    application_list = [f for f in os.listdir("client/applications/") if not os.path.isfile(f)]
    if(app_name in application_list):
        return importlib.import_module("client.applications." + app_name + ".main")
