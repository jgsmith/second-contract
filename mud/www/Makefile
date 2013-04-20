TMPL_DIR = ./templates
JS_DIR = ./js

TEMPLATES = ${TMPL_DIR}/*.haml
MODALS = ${TMPL_DIR}/modal/*.haml
VIEWS = ${JS_DIR}/views/*.coffee
MODELS = ${JS_DIR}/models/*.coffee

all: index.html ${JS_DIR}/sc-admin.js

${JS_DIR}/sc-admin.js: ${JS_DIR}/sc-admin.coffee.pre ${JS_DIR}/sc-admin.coffee.post ${VIEWS} ${MODELS}
	@@echo "Building sc-admin.js"
	@@rm -f ${JS_DIR}/sc-admin.coffee
	@@cp ${JS_DIR}/sc-admin.coffee.pre ${JS_DIR}/sc-admin.coffee
	@@echo >> ${JS_DIR}/sc-admin.coffee
	@@for i in ${MODELS} ${VIEWS}; do \
		cat $$i | sed 's/^/  /' >> ${JS_DIR}/sc-admin.coffee; \
		echo >> ${JS_DIR}/sc-admin.coffee; \
		done
	@@cat ${JS_DIR}/sc-admin.coffee.post >> ${JS_DIR}/sc-admin.coffee
	@@/usr/local/bin/coffee -c ${JS_DIR}/sc-admin.coffee

index.html: index.haml
	@@echo "Building index.html"
	/usr/bin/haml index.haml > index.html

index.haml: index.haml.pre index.haml.post ${TEMPLATES} ${MODALS}
	@@echo "Building index.haml"

	@@cp index.haml.pre index.haml
	@@for i in ${TEMPLATES}; do \
		cat $$i | sed 's/^/      /' >> index.haml; \
		echo >> index.haml; \
	done
	@@for i in ${MODALS}; do \
		cat $$i | sed 's/^/    /' >> index.haml; \
		echo >> index.haml; \
	done
	@@cat index.haml.post >> index.haml
