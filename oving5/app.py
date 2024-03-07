from flask import Flask, request, jsonify
from flask_cors import CORS
import contextlib
import io

app = Flask(__name__)
# Tillater CORS-forespørsler fra et spesifikt opphav
cors = CORS(app, resources={r"/compile": {"origins": "http://localhost:63345"}})

@app.route('/compile', methods=['POST'])
def compile_code():
    code = request.json.get('code')
    result = io.StringIO()  # Oppretter et streng-basert IO-objekt for å fange output
    try:
        with contextlib.redirect_stdout(result):
            exec(code, {})
        output = result.getvalue()  # Henter output som en streng
        return jsonify({"result": output})
    except Exception as e:
        return jsonify({"error": str(e)})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
