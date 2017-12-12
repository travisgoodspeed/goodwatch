
all: theme


theme:
	mkdir -p themes
	cd themes && ls hugo_theme_robust || git clone https://github.com/dim0627/hugo_theme_robust.git
server: theme
	hugo server
deploy:
	hugo
	cp -rf public/* /var/www/html/

