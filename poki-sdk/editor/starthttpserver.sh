
BUNDLE_OUTPUT=$1
PORT=$2
PIDFILE=.poki-pid
ZIPFILE=poki.zip

# zip bundle output
if [ -f ${BUNDLE_OUTPUT}/${ZIPFILE} ]; then
	rm ${BUNDLE_OUTPUT}/${ZIPFILE}
fi
(cd ${BUNDLE_OUTPUT}; zip -r poki.zip .;)

# check if there's a saved server pid
if [ -f ${PIDFILE} ]; then
	PID=$(<${PIDFILE})
	echo "FOUND PID ${PID}"
	kill ${PID}
	rm ${PIDFILE}
fi

nohup /usr/local/bin/python poki-sdk/editor/httpserver.py ${BUNDLE_OUTPUT} ${PORT} &
# nohup /usr/local/bin/python -m http.server -d ${BUNDLE_OUTPUT} ${PORT}
PID=$!

echo "pid ${PID}"
echo "${PID}" > ${PIDFILE}

open https://app.poki.dev/upload-defold?project=MYPROJECT&zipfile=http://127.0.0.1:${PORT}/${ZIPFILE}
