

meta:
  id: frisquet
  file-extension: frisquet
  endian: le

seq:
  - id: payloads
    type: payload
    repeat: eos
    
types:
  payload:
    seq:
      - id: length
        type: u1
      - id: to_addr
        type: u1
      - id: from_addr
        type: u1
      - id: request_id
        type: u2
        doc: Est contenu dans la reponse
      - id: req_or_answer
        type: u1
        doc: Vaut requete 01 (satellite) ou reponse 81 (chaudiere)
      - id: msg_type
        type: u1
      - id: payload_data
        size: payload_data_size
        type: 
          switch-on: from_addr
          cases:
            0x8: satellite 
            0x09: satellite
            0x0a: satellite
            0x80: chaudiere
            0x20: sonde
            0x7E: connect
          # switch-on: length
          # cases:
          #   23: set_temperature
          #   49: set_temperature_response
          #   10: unknown_msg
          #   55: unknown_msg_response
          #   17: init
          #   8: init_empty_message
          #   6: init_empty_message_response
          #   _: dummy
    instances:
      payload_data_size: 
        value: length -6
  satellite:
    seq:
      - id: satellite_data
        type: 
          switch-on: _parent.length
          cases: 
            23: set_temperature
            17: init
            10: unknown_msg
            8: init_empty_message
            _: dummy_satellite
  chaudiere:
    seq:
      - id: chaudiere_data
        type: 
          switch-on: _parent.length
          cases: 
            15: sonde_response
            49: set_temperature_response
            55: unknown_msg_response
            6: init_empty_message_response
            _: dummy_chaudiere
  sonde:
    seq:
      - id: sonde_data
        type: 
          switch-on: _parent.length
          cases: 
            17: sonde_temperature
            _: dummy_sonde

  connect:
    seq:
      - id: connect_data
        type: 
          switch-on: _parent.length
          cases: 
            _: dummy_connect
  set_temperature:
    seq: 
      - id: static_part_start
        #contents: [160, 41, 0]
        size: 3
      - id: unknown1
        type: u1
        doc: 24 sur les chaudieres sans connect.
      - id: static_part_end
        #contents: [160, 47, 0]
        size: 3
        doc: Semble etre statique sur toutes les chaudieres sans connect = [160, 41, 0, 24, 160, 47, 0, 4, 8, 0]
      - id: unknown2
        type: u1
      - id: message_static_part
        size: 2
        #contents: [8,0]
      - id: temperature
        type: u2
        doc: temperature en degre * 100. Verifier au dessus de 25.5
      - id: consigne
        type: u2
        doc: temperature en degre * 100. Verifier au dessus de 25.5
      - id: unknown_mode1
        type: b3
      - id: hors_gel
        type: b1
      - id: unknown_mode2
        type: b2
      - id: derogation
        type: b1
      - id: soleil
        type: b1
      - id: signature
        doc: Provient de set_temperature_response.payload.signature[1,2] precedente
        size: 2
        
  set_temperature_response:
    seq: 
    
      - id: unknown_start
        size: 2
      - id: temperature_exterieure
        type: u2
        doc: Semble etre statique sur toutes les chaudieres.
      - id: year
        type: u1
      - id: month
        type: u1
      - id: day
        type: u1
      - id: hour
        type: u1
      - id: minute
        type: u1
      - id: second
        type: u1
      - id: unknown1
        doc: varie peu, entre [8,[1,2,3],0]
        size: 3
      - id: temperature
        type: u2
      - id: consigne
        type: u2
      - id: unknown_conso
        doc: la conso peut-être ?
        type: u2
      - id: signature
        doc: contient SetTemperature.signature a l'envers. aa, bb => bb, aa, xx. [bb, xx] sera la prochaine signature a fournir.
        size: 3
      - id: static_part_2
        doc: Identique dans toutes les chaudieres.
        size: 19
      - id: remaining_data
        # size: _parent._parent.payload_data_size - 42
        size-eos: true
        
  init:
    doc: Quand un satellite demarre, il envoie cette payload
    seq:

      - id: static_part
        size: 7
        doc: Semble etre statique sur toutes les chaudieres = [160, 41, 0, 21, 160, 47, 0, 4, 8, 0]
      - id: message_static_part
        size: 3
        doc: semble varier, x, 2*x, 0, mais dépendant du type de message
      - id: data
        size-eos: true
  unknown_msg:
    doc: Une fois par jour, 3 fois de suite, toujours [[26, 50, 74], 0, 24]
    seq:
      - id: data
        size-eos: true
  unknown_msg_response:
    seq:
      - id: data
        size-eos: true
  init_empty_message:
    doc: A l'init, message qui a l'air vide ou presque
    seq:
      - id: data
        size-eos: true
  init_empty_message_response:
    doc: A l'init, message qui a l'air vide ou presque
    seq:
      - id: data
        size-eos: true
  sonde_temperature:
    seq:
      - id: data
        size: 9
      - id: temperature_sign
        type: u1
      - id: temperature
        type: u1
  
  sonde_response:
    seq:
      - id: unknown_start
        type: u1
      - id: year
        type: u1
      - id: month
        type: u1
      - id: day
        type: u1
      - id: hour
        type: u1
      - id: minute
        type: u1
      - id: second
        type: u1
      - id: data
        size-eos: true
  dummy_satellite: {}
  dummy_chaudiere: {}
  dummy_sonde: {}
  dummy_connect: {}
        
        
