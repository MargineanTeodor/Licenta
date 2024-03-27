import pandas as pd
import sys

if len(sys.argv) < 2:
    print("Usage: python script.py <max_rows>")
    sys.exit(1) # check if we get the number of rows 

max_rows = int(sys.argv[1])

df = pd.read_csv('data.csv', nrows=max_rows)
#create xlsx

excel_writer = pd.ExcelWriter('data.xlsx', engine='xlsxwriter')
df.to_excel(excel_writer, sheet_name='PositionPredictionAccuracy')

workbook = excel_writer.book
worksheet = excel_writer.sheets['PositionPredictionAccuracy']
chartX = workbook.add_chart({'type': 'line'})
chartY = workbook.add_chart({'type': 'line'})
chartZ = workbook.add_chart({'type': 'line'})

#X chart
chartX.add_series({
    'values': f'=PositionPredictionAccuracy!$B$2:$B${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$B$1',
})
chartX.add_series({
    'values': f'=PositionPredictionAccuracy!$E$2:$E${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$E$1',
})
chartX.add_series({
    'values': f'=PositionPredictionAccuracy!$H$2:$H${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$H$1',
})

#Y chart 
chartY.add_series({
    'values': f'=PositionPredictionAccuracy!$C$2:$C${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$C$1',
})
chartY.add_series({
    'values': f'=PositionPredictionAccuracy!$F$2:$F${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$F$1',
})
chartY.add_series({
    'values': f'=PositionPredictionAccuracy!$I$2:$I${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$I$1',
})

#Z chart 
chartZ.add_series({
    'values': f'=PositionPredictionAccuracy!$D$2:$D${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$D$1',
})
chartZ.add_series({
    'values': f'=PositionPredictionAccuracy!$G$2:$G${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$G$1',
})
chartZ.add_series({
    'values': f'=PositionPredictionAccuracy!$J$2:$J${max_rows+1}',
    'name': '=PositionPredictionAccuracy!$J$1',
})

worksheet.insert_chart('M2', chartX)
worksheet.insert_chart('M20', chartY)
worksheet.insert_chart('M38', chartZ)

# Save the Excel file
excel_writer._save()
