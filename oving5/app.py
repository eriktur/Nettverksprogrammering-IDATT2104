from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
import os

app = Flask(__name__)
CORS(app)


@app.route('/compile', methods=['POST'])
def compile_code():
    code = request.json.get('code')
    # Lagre mottatt kode til en midlertidig fil
    with open("temp_script.py", "w") as file:
        file.write(code)

    # Kjør Python-koden i en ny Docker-container
    try:
        result = subprocess.run(
            ["docker", "run", "--rm", "-v", f"{os.getcwd()}:/app", "python-runner", "python", "/app/temp_script.py"],
            capture_output=True, text=True, timeout=30)
        output = result.stdout if result.stdout else result.stderr
        return jsonify({"result": output})
    except subprocess.TimeoutExpired:
        return jsonify({"error": "Timeout - Koden tok for lang tid å kjøre"})
    except Exception as e:
        return jsonify({"error": str(e)})


if __name__ == '__main__':
    app.run(debug=True, port=5000)
