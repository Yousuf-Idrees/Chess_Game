import http.server
import socketserver
import json
import subprocess
import os

PORT = 8080
DIRECTORY = os.path.dirname(os.path.abspath(__file__))

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)

    def do_POST(self):
        if self.path.startswith('/api/'):
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            req = json.loads(post_data.decode('utf-8'))
            
            cmd = req.get('cmd')
            args = req.get('args', [])
            
            exe_path = os.path.join(os.path.dirname(DIRECTORY), "ChessGame.exe")
            if not os.path.exists(exe_path):
                self.send_response(500)
                self.end_headers()
                self.wfile.write(b'{"error": "ChessGame.exe not found. Compile it first with build.bat"}')
                return

            try:
                process = subprocess.run([exe_path, "gui", cmd] + args, capture_output=True, text=True)
                output = process.stdout.strip()
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(output.encode('utf-8'))
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

with socketserver.TCPServer(("", PORT), Handler) as httpd:
    print(f"GUI Server running at http://localhost:{PORT}")
    print("Open this link in your web browser to play!")
    httpd.serve_forever()
