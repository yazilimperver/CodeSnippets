/**
 * @file   CBoundedSharedQueue.h
 * @author yazılımperver
 * @brief  Sınırlı paylaşımlı temel kuyruk. Bu durumda, sinirsiz kuyruktan farkli olarak
 * 		   kapasite doldugu zaman da cagrilari locklayacak, bu sebeple iki adet condition variable
 * 		   kullanilmaktadir. Diyelim ki, kuyruk doldu, bekletiyoruz ve pop ettigimizde ayrica push
 * 		   cagrisi sonucu, kuyruk doldugu icin bekleyen kardesimizi de uyandiriyoruz.
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

/*! Temel bir (siniri olmayan) paylasimli kuyruk */
template <typename T>
class CBoundedSharedQueue 	{
public:
	/*! @brief Construct a new CBoundedSharedQueue object */
	CBoundedSharedQueue(std::size_t maxSize)
		: mMaxSize{ maxSize } {		
	}

	~CBoundedSharedQueue(void) {
		Invalidate();
	}

	/*! @brief Kuyruktaki ilk elemani almaya calisir. Alabilirse dogru doner. */
	bool TryPop(T& out) {
		std::lock_guard<std::mutex> lock(mGuardMutex);
		if (mQueue.empty() || !mValid)
		{
			return false;
		}
		out = std::move(mQueue.front());
		mQueue.pop();

		mPushConditionVariable.notify_one();		
		return true;
	}

	/*! @brief Ilk elemani almaya calisir. Eger bos ise ilk eleman gelene ya da gecerliligini yitirene kadar bekler.	 */
	bool Pop(T& out) {
		{
			std::unique_lock<std::mutex> lock(mGuardMutex);
			mPopConditionVariable.wait(lock, [this]() { return !this->mQueue.empty() || !mValid; });

			if (!mValid) {
				return false;
			}
			out = std::move(mQueue.front());
			mQueue.pop();
		}

		mPushConditionVariable.notify_one();
		return true;
	}

	/*! @brief Ilk elemani alir. Eger bos ise verilen sure kadar bekler. Basarili olursa dogru doner. */
	bool WaitPop(T& out, std::chrono::milliseconds waitDuration = std::chrono::milliseconds{1000}) {
		{
			std::unique_lock<std::mutex> lock(mGuardMutex);
			
			if (!mPopConditionVariable.wait_for(lock, waitDuration, [this]() { return !this->mQueue.empty() || !mValid; })) {
				return false;
			}

			if (!mValid) {
				return false;
			}
			out = std::move(mQueue.front());
			mQueue.pop();
		}

		mPushConditionVariable.notify_one();
		return true;
	}
	
	/*! @brief	Yeni kaydi kuyruha ekler */
	bool Push(const T& value) {
		{
			std::lock_guard<std::mutex> lock(mGuardMutex);
			mPushConditionVariable.wait(lock, [&](){ return mQueue.size() < mMaxSize || !mValid; });

			if (!mValid) {
				return false;
			}

			mQueue.push(value);
		}
	
		mPopConditionVariable.notify_one();
		return true;
	}

	/*! @brief	Yeni kaydi kuyruga ekler */
	bool Push(T&& value) {
		{
			std::lock_guard<std::mutex> lock(mGuardMutex);
			mPushConditionVariable.wait(lock, [&](){ return mQueue.size() < mMaxSize || !mValid; });

			if (!mValid) {
				return false;
			}

			mQueue.push(std::move(value));
		}
	
		mPopConditionVariable.notify_one();
		return true;
	}

	/*! @brief	Kuyruk bos mu degil mi, kontrolu yapar. */
	bool IsEmpty(void) const {
		std::lock_guard<std::mutex> lock(mGuardMutex);
		return mQueue.empty();
	}

	/*! @brief	Clear all items from the queue. */
	void Clear(void) {
		std::lock_guard<std::mutex> lock(mGuardMutex);
		while (!mQueue.empty()) {
			mQueue.pop();
		}
		mPushConditionVariable.notify_all();
		mPopConditionVariable.notify_all();
	}

	/*! @brief Kuyrugu gecersiz kilmaya yonelik islevleri gerceklestirir. Butun bekleyenleri uyandirir.	 */
	void Invalidate(void) {
		std::lock_guard<std::mutex> lock{ mGuardMutex };
		mValid = false;
		mPushConditionVariable.notify_all();
		mPopConditionVariable.notify_all();
	}

	/*! @brief Kuyrugun gecerli olup/olmadigini doner.*/
	bool IsValid(void) const {
		std::lock_guard<std::mutex> lock{ mGuardMutex };
		return mValid;
	}

private:
	/*! @brief	Uygulama cikarken kuyrugun gecersiz oldugunu ifade etmek icin kullanilacak degisken */
	std::atomic_bool mValid{ true };

	/*! @brief	Const fonksiyonlar icin mutable yapildi. CV ve diger kitlemelerde kullanilacak */
	mutable std::mutex mGuardMutex;
	
	/*! @brief	Altta kullanacagimiz kuyruk  */
	std::queue<T> mQueue;
	
	/*! @brief	Pop icin kullanilacak, kuyrugu uyandirmak ve uyuytmak icin kullanacagimiz kosul degiskeni */
	std::condition_variable mPopConditionVariable;
	
	/*! @brief Push icin kullanilacak, kuyrugu uyandirmak ve uyutmak icin kullanacagimiz kosul degiskeni */
	std::condition_variable mPushConditionVariable;

	/*! @brief En fazla alabilecegi boyut */
	std::size_t mMaxSize;
};