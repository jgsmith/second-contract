TMPL_DIR = ./templates
JS_DIR = ./js

TEMPLATES = ${TMPL_DIR}/iflib.haml \
	${TMPL_DIR}/system.haml \
	${TMPL_DIR}/verb.haml \
	${TMPL_DIR}/verb-list.haml \
	${TMPL_DIR}/verb-list-item.haml \
	${TMPL_DIR}/worldlib.haml

all: index.html ${JS_DIR}/sc-admin.js

${JS_DIR}/sc-admin.js: ${JS_DIR}/sc-admin.coffee
	@@echo "Building sc-admin.js"
	@@/usr/local/bin/coffee -c ${JS_DIR}/sc-admin.coffee

index.html: index.haml
	@@echo "Building index.html"
	/usr/bin/haml index.haml > index.html

index.haml: index.haml.pre index.haml.post ${TEMPLATES}
	@@echo "Building index.haml"

	@@cp index.haml.pre index.haml
	@@for i in ${TEMPLATES}; do \
		cat $$i | sed 's/^/      /' >> index.haml; \
		echo >> index.haml; \
	done
	@@cat index.haml.post >> index.haml
