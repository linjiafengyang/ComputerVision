import csv
import struct
import os
import numpy as np
from PIL import Image
from sklearn.metrics import accuracy_score
from sklearn.neural_network import MLPClassifier
from sklearn.externals import joblib
import pandas as pd

def load_mnist(path, kind='train'):
	# 加载mnist数据集
	labels_path = os.path.join(path, '%s-labels.idx1-ubyte' % kind)
	images_path = os.path.join(path, '%s-images.idx3-ubyte' % kind)
	with open(labels_path, 'rb') as lbpath:
		magic, n = struct.unpack('>II',
                                 lbpath.read(8))
		labels = np.fromfile(lbpath, dtype=np.uint8)

	with open(images_path, 'rb') as imgpath:
		magic, num, rows, cols = struct.unpack('>IIII',
                                               imgpath.read(16))
		images = np.fromfile(imgpath, dtype=np.uint8).reshape(len(labels), 784)

	return images, labels

def train_model(mlp, save_dir):
	# 加载训练数据集
	[train_images, train_labels] = load_mnist('./mlp/data/')
	train_labels = train_labels.astype('int32')
	train_images = train_images.astype('int32')
	# 开始训练
	mlp.fit(train_images, train_labels)
	# 保存模型
	joblib.dump(mlp, save_dir)
	mlp = joblib.load(save_dir)
	# 加载测试数据集
	[test_images, test_labels] = load_mnist('./mlp/data/', kind = 't10k')
	test_labels = test_labels.astype('int32')
	test_images = test_images.astype('int32')
	# 预测测试数据集
	y_pred = list(mlp.predict(test_images))
	print(accuracy_score(test_labels, y_pred)) # 测试集的得分

mlp = MLPClassifier(hidden_layer_sizes=(400, 200), activation='logistic', 
				solver='sgd', learning_rate_init=0.001, max_iter=400, verbose = True)

save_dir = './mlp/model/classify.m'
if(os.path.isfile(save_dir)):
	# 加载模型
	mlp = joblib.load(save_dir)
else:
	train_model(mlp, save_dir)

print('Reading images directory from txt file...')
imageDir = []
f = open('./imageDir.txt')
imagedir = f.readline()
while imagedir:
	imagedir = imagedir.strip('\n')
	imageDir.append(imagedir)
	imagedir = f.readline()
f.close()

print('Done! Reading images from directory one by one...')
for dir in imageDir:
	images = []
	try:
		print('Done! Reading images from directory: %s' % dir)
		f = open(dir + '/imagelist.txt')
		image = f.readline()
		while image:
			image = image.strip('\n')
			images.append(image)
			image = f.readline()
		f.close()
	except:
		print('No such file, continue...')
	finally:
		print('Predicted results:')
		results = ""
		final_results = []
		for i in images:
			if i != '*':
				filename = dir + '/' + i
				img = Image.open(filename)
				img = img.resize((28,28))
				arr = np.array(img)
				arr = arr[:,:,:1]
				arr = arr.reshape(1, 784)
				res = int(mlp.predict(arr.copy()))
				results = results + str(res)
			else:
				print(results)
				final_results.append(results)
				results = ""

		writer = pd.ExcelWriter('excel/' + dir[-9:] +'.xlsx', engine='xlsxwriter')
		df = pd.DataFrame({'number': final_results})
		df.to_excel(writer, sheet_name='Sheet1', index=False)
		writer.save()
		writer.close()
