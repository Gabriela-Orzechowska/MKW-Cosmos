import os.path
import api

from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError

SPREADSHEET_ID = '1zTk-rzbwMALg9jELV9rHoA6CQkWYCSUXRXp49mzfYRI'

if __name__ == '__main__':
    sheets = build('sheets', 'v4', developerKey=api.API_KEY).spreadsheets();
    for lang in range(12):
        lang += 1
        ret = sheets.values().get(spreadsheetId=SPREADSHEET_ID, range="WiiLink WFC Edits!A:N").execute()

        with open(f"""output-{ret['values'][0][lang+1]}.txt""", 'a', encoding="utf-8") as f:
            for i in ret['values'][2:]:
                ext = i[0] + '  = ' + i[lang+1].replace('\n','\\n')
                ext = ext.replace('\n','\\n')
                f.write(ext)
                f.write('\n')
            f.write('\n\n')

        ret = sheets.values().get(spreadsheetId=SPREADSHEET_ID, range="Aurora!A:N").execute()

        with open(f"""output-{ret['values'][0][lang+1]}.txt""", 'a', encoding="utf-8") as f:
            for i in ret['values'][2:]:
                ext = i[0] + '  = ' + i[lang+1].replace('\n','\\n')
                ext = ext.replace('\n','\\n')
                ext = ext.replace('= ?','= ')
                f.write(ext)
                f.write('\n')

