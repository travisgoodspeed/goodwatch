
all: build


theme:
	mkdir -p themes
	cd themes && ls hugo_theme_robust || git clone https://github.com/dim0627/hugo_theme_robust.git
build: theme
	hugo
server: theme
	hugo server
deploy: build
	cp -rf public/* /var/www/html/

clean:
	rm -rf public
