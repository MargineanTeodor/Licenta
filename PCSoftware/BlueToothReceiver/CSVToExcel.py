import pandas as pd

# Read the CSV file
df = pd.read_csv('data.csv')

# Save as an Excel file
excel_writer = pd.ExcelWriter('data.xlsx', engine='xlsxwriter')
df.to_excel(excel_writer, sheet_name='Sheet1')

# Optionally, create a chart using XlsxWriter (pandas' Excel writer engine uses XlsxWriter under the hood)
workbook  = excel_writer.book
worksheet = excel_writer.sheets['Sheet1']
chart = workbook.add_chart({'type': 'line'})

# Configure the series of the chart from the dataframe data.
# chart.add_series({
#     'values': '=Sheet1!$B$2:$B$5',
#     'name': '=Sheet1!$B$1',
# })
chart.add_series({
    'values': '=Sheet1!$C$2:$C$5',
    'name': '=Sheet1!$C$1',
})
worksheet.insert_chart('D2', chart)

excel_writer._save()
