#!/usr/bin/python3
from slacker import Slacker
import argparse
from os import curdir
from os.path import join as pjoin
import http.server
import uuid
import ssl
import subprocess

PORT = 8000 #TODO  random port for multiple connections attemp
ACK_URL = "/" + str(uuid.uuid4())
parser = argparse.ArgumentParser()
parser.add_argument("username", help="give an username")
parser.add_argument("ifname", help="giva an pppX iface name")
args = parser.parse_args()

def ipt_add():
  p = subprocess.Popen(["iptables", "-I", "FORWARD", "-i", args.ifname,"-m", "comment", "--comment", ACK_URL, "-j", "DROP"], stdout=subprocess.PIPE)
  output , err = p.communicate()
def ipt_del():
  p = subprocess.Popen(["iptables", "-D", "FORWARD", "-i", args.ifname,"-m", "comment", "--comment", ACK_URL, "-j", "DROP"], stdout=subprocess.PIPE)
  output , err = p.communicate()
def stop_ppp():
  f = open('/var/run/'+args.ifname+'.pid')
  PID = f.read().split('\n')
  f.close()
  p = subprocess.Popen(["kill", "-9", PID[0]], stdout=subprocess.PIPE)
  output , err = p.communicate()

ipt_add()
slack = Slacker('<put here slack token>')
slack.chat.post_message('@'+args.username, '*<https://<Your url for ACK>:'+str(PORT)+ACK_URL+'|VPN ACK>*')

class Handler(http.server.SimpleHTTPRequestHandler):
  def do_GET(self):
    try:
      if self.path == ACK_URL:
        self.send_response(200)
        self.send_header('Content-type','text/plain')
        self.end_headers()
        self.wfile.write(bytes('OK!\n','utf8'))
        ipt_del()
      else:
        self.send_response(403)
        self.end_headers()
      return
    except:
      self.send_response(403)
      self.end_headers()
    finally:
      quit()

httpd = http.server.HTTPServer(("", PORT), Handler)
httpd.socket = ssl.wrap_socket (httpd.socket, keyfile='<ssl key>', certfile='<ssl crt>', server_side=True)
httpd.timeout = 60
httpd.handle_request()
httpd.server_close()
ipt_del()
stop_ppp()
