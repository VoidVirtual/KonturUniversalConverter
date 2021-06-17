import json
import os
import http.client
from http import HTTPStatus


class Response:
    def __init__(self, body, status):
        self.body = body
        self.status = status

    def __eq__(self, other):
        return self.body == other.body and self.status == other.status


class TestingClient:
    def __init__(self, logs=True):
        self.logs = logs
        self.passed = 0
        self.cases = 0

    def send_request(self, expression, uri_path):
        connection = http.client.HTTPConnection('localhost', 80)
        headers = {'Content-type': 'application/json'}
        connection.request('POST', uri_path, expression, headers)
        response = connection.getresponse()
        return Response(response.read().decode(), response.status)

    def get_test_params(self, filename):
        file = open(filename)
        case_json = json.load(file)
        uri_path = str(case_json.get('path'))
        result_dict = case_json.get('result')
        status = result_dict.get('status')
        body = json.dumps(result_dict.get('body'))
        expression = json.dumps(case_json['expression'])
        file.close()
        return expression, Response(body, status), uri_path

    def test_case(self, filepath):
        expression, expected, uri_path = self.get_test_params(filepath)
        response = self.send_request(expression, uri_path)
        if response.status == HTTPStatus.OK:
            test_passed = (response == expected)
        else:
            test_passed = (expected.status == response.status)
        if self.logs and not test_passed:
            self.log_fail(filepath, expected, response)
        if test_passed:
            self.passed += 1
        self.cases += 1

    def log_fail(self, filepath, expected, response):
        expected_status_phrase = HTTPStatus(expected.status).phrase
        response_status_phrase = HTTPStatus(response.status).phrase
        print('Test failed ("' + filepath + '")')
        print('Expected: ' + expected.body + ' ' + expected_status_phrase)
        print('Result: ' + response.body + ' ' + response_status_phrase)
        print("")

    def log_results(self):
        print('___________________________________________________________________________')
        print('Tests passed: '+str(self.passed)+' of '+str(self.cases))
        print('___________________________________________________________________________')

def test_all_cases(cases_dir):
    client = TestingClient()
    for filename in os.listdir(cases_dir):
        if filename.endswith(".json"):
            try:
                filepath = cases_dir+'/'+filename
                client.test_case(filepath)
            except ConnectionRefusedError:
                print('Server is not responding')
                break
            except FileNotFoundError:
                print(FileNotFoundError.filename + 'is not found')
            continue
        else:
            continue
    client.log_results()
