keytool -genkeypair -alias signFiles -keyalg RSA -keystore examplestore -storepass DITT_PASSORD -validity 365 -keysize 2048

javac JavaSSLServer.java JavaSSLClient.java

java -D"javax.net.ssl.keyStore=examplestore" -D"javax.net.ssl.keyStorePassword=DITT_PASSORD" JavaSSLServer

java -D"javax.net.ssl.trustStore=examplestore" -D"javax.net.ssl.trustStorePassword=DITT_PASSORD" JavaSSLClient
