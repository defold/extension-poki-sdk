#!/usr/bin/env python3

import sys
import os
import http.server
import socketserver

DIRECTORY = sys.argv[1]
PORT = int(sys.argv[2])

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)

    # GET
    def do_GET(self):
        try:
            f = open(os.path.join(self.directory, self.path[1:]), 'rb')
            body = f.read()

            # Send response status code
            self.send_response(200)

            # Send headers
            self.send_header('Access-Control-Allow-Origin', '*')
            self.send_header('Content-type','application/zip')
            self.send_header('Content-length', str(len(body)))
            self.end_headers()

            # Send body
            self.wfile.write(body)

        except OSError as e:
            print("do_GET error", e)
            self.send_response(404)
            self.end_headers()

    def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS')
        self.end_headers()

def run():
    # print('Starting server...', PORT, DIRECTORY)

    server_address = ('127.0.0.1', PORT)

    try:
        with socketserver.TCPServer(("", PORT), Handler) as httpd:
            print('Running server...')
            httpd.serve_forever()
    except OSError as e:
        print("Server error", e)

run()