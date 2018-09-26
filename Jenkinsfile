pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building firmware.'
		writeFile file: "firmware/config.h", text: "#define CALLSIGN \"TEST\""
		dir("firmware") {
		    sh "make clean all"
		}
            }
        }
        stage('UART Test') {
            steps {
		dir("firmware") {
		    sh "make flash"
		}
            }
        }
        stage('SBW Test') {
            steps {
		dir("firmware") {
		    sh "make energytrace.txt"
		}
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
            }
        }
    }
}
