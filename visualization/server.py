import os.path
import tornado.auth
import tornado.escape
import tornado.httpserver
import tornado.ioloop
import tornado.options
import tornado.web

from tornado.options import define, options
# local imports
# ... not using

define("port", default=8080, help="run on the given port", type=int)
# define("facebook_api_key", help="your Facebook application API key", type=str)
# define("facebook_secret", help="your Facebook application secret", type=str)

class Application(tornado.web.Application):
    def __init__(self):
        handlers = [
            (r"/", MainHandler),
            (r"/historical_data", HistoricalDataHandler),
            # (r"/auth/logout", AuthLogoutHandler),
        ]
        settings = dict(
            cookie_secret = "RANDOMCOOKIEHASH",
            xsrf_cookies = True,
            # login_url="/auth/login",
            # ui_modules={"Post": PostModule},
            template_path = os.path.join(os.path.dirname(__file__), "templates"),
            static_path = os.path.join(os.path.dirname(__file__), "static"),
            debug = True,
            autoescape = None,
        )
        tornado.web.Application.__init__(self, handlers, **settings)

# class MiddleWareHandler(tornado.web.RequestHandler):

class MainHandler(tornado.web.RequestHandler):
    async def get(self):
        self.write("index >> no use")

class HistoricalDataHandler(tornado.web.RequestHandler):
    async def get(self):
        # self.write(template_rendered)
        ticker = {
            "horizon": "201901-201908",
            "name": "BTCUSDT",
            "interval": "M1",
            "series": ""
        }
        with open(f"../resources/data/{ticker['name']}_{ticker['horizon']}.json", 'r') as file:
            ticker["series"] = file.read() #.replace('\n', '')
        self.render("historical_data.html",
            title = ticker["name"] + " Historical Prices",
            ticker = ticker)

def main():
    tornado.options.parse_command_line()
    http_server = tornado.httpserver.HTTPServer(Application())
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.current().start()

if __name__ == "__main__":
    main()
