document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('run-btn').addEventListener('click', function() {
        const inputText = document.getElementById('input').value;
        fetch('http://127.0.0.1:5000/compile', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ code: inputText })
        })
        .then(response => response.json())
        .then(data => {
            document.getElementById('output').textContent = data.result || data.error;
        })
        .catch(error => console.error('Error:', error));
    });
});
