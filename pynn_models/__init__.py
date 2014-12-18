from recurrent_rule import RecurrentRule

def _init_module():
    import logging
    import os
    import spynnaker.pyNN
    
    # Determine path of recurrent rule model binaries directory
    binary_path = os.path.abspath(recurrent_rule.__file__)
    binary_path = os.path.abspath(os.path.join(binary_path, os.pardir))
    binary_path = os.path.join(binary_path, "neural_modelling","build")

    # Register this path with SpyNNaker
    spynnaker.pyNN.register_binary_search_path(binary_path)

_init_module()